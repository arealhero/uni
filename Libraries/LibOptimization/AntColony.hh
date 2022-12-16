#pragma once

#include <LibMath/Filters.hh>
#include <LibMath/Graph.hh>
#include <LibStatistics/Numbers.hh>
#include <VS/Pair.hh>
#include <VS/Traits.hh>
#include <VS/Types.hh>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <set>
#include <thread>

namespace Uni
{

template <typename Traits>
concept AntColonyTraits = requires(const VS::Indices& path)
{
  typename Traits::Weight;
  typename Traits::PathLength;

  requires requires(const Uni::DirectedGraph<typename Traits::Weight>& graph)
  {
    {
      Traits::calculate_path_length(graph, path)
      } -> std::same_as<typename Traits::PathLength>;
  };

  requires requires(const VS::Vector<typename Traits::PathLength>& lengths)
  {
    {
      Traits::find_best_path_index(lengths)
      } -> std::same_as<VS::Index>;
  };
};

template <AntColonyTraits Traits>
class AntColony
{
 public:
  // FIXME: get from Traits?
  using Number = double;

  using Path = VS::Indices;
  using Weight = typename Traits::Weight;
  using PathLength = typename Traits::PathLength;

  struct Params
  {
    std::uint32_t seed;

    Number pheromone_weight;
    Number desirability_weight;
    Number initial_pheromone_level;
    Number pheromone_contribution;
    Number minimal_pheromone_level;
    Number evaporation_rate;

    VS::Index start_node_index;

    std::size_t ants;
    std::size_t iterations;
  };

  struct EdgeWeight
  {
    Number desirability;
    Number pheromone_level;
  };

  using AntGraph = DirectedGraph<EdgeWeight>;

  // FIXME: move to JobShopBuilder or something
  enum class PriorityRule
  {
    SPT,  // Select the operation with shortest processing time
    LPT,  // Select the operation with longest processing time
    SRT,  // Select the operation belonging to the job with the shortest
          // remaining processing time
    LRT,  // (*) Select the operation belonging to the job with the longest
          // remaining processing time
    LRM,  // Select the operation belonging to the job with the shortest
          // remaining processing time excluding the operation under
          // consideration
  };

  struct ControlData
  {
    PathLength min;
    PathLength mean;
    PathLength max;
  };

  struct Result
  {
    struct
    {
      Path path;
      PathLength length;
    } best;
    VS::Vector<Path> best_paths;
    VS::Vector<PathLength> path_lengths;
    VS::Vector<ControlData> controls;
  };

  constexpr explicit AntColony(const Params& params) : m_params(params) {}

  // FIXME: think of a return type
  [[nodiscard]] constexpr auto run(const DirectedGraph<Weight>& graph) const
      -> Result
  {
    assert(graph.number_of_nodes() > 1);
    auto ant_graph = build_ant_graph(graph);

    // Statistics collection
    auto path_lengths = VS::Vector<PathLength>{};
    auto iteration_controls = VS::Vector<ControlData>{};
    auto best_paths = VS::Vector<Path>{};

    Path best{};
    PathLength best_path_length = 100'000'000;

    path_lengths.reserve(iterations() * ants());
    iteration_controls.reserve(iterations());
    best_paths.reserve(iterations());

    for (VS::Index i = 0; i < iterations(); ++i)
    {
      std::cout << "Iteration " << i << '\n';
      // FIXME: change to `Matrix<Number>`?
      auto contributions = VS::Vector<VS::Vector<Number>>(
          graph.number_of_nodes(),
          VS::Vector<Number>(graph.number_of_nodes(), Number{0}));

      auto current_paths = VS::Vector<Path>{};
      auto current_lengths = VS::Vector<PathLength>{};

      current_paths.reserve(ants());
      current_lengths.reserve(ants());

      for (VS::Index ant_index = 0; ant_index < ants(); ++ant_index)
      {
        const auto ant = Ant{get_params(), ant_index};
        const auto path = ant.walk(ant_graph);
        const auto path_length = calculate_path_length(graph, path);

        for (VS::Index j = 0; j < path.size() - 1; ++j)
        {
          const auto current_index = path.at(j);
          const auto next_index = path.at(j + 1);

          // FIXME: make this division optional (sometimes ACO should prefer
          // longer paths)
          contributions.at(current_index).at(next_index) +=
              pheromone_contribution() / path_length;
        }

        if (path_length < best_path_length)
        {
          best = path;
          best_path_length = path_length;
        }

        current_paths.push_back(path);
        path_lengths.push_back(path_length);
        current_lengths.push_back(path_length);
      }

      const auto best_path_index = find_best_path_index(current_lengths);
      best_paths.push_back(current_paths.at(best_path_index));

      iteration_controls.push_back(
          {min(current_lengths), mean(current_lengths), max(current_lengths)});

      update_pheromones(ant_graph, contributions);
    }

    return {{best, best_path_length},
            best_paths,
            path_lengths,
            iteration_controls};
  }

  [[nodiscard]] constexpr auto get_params() const -> const Params&
  {
    return m_params;
  }

  [[nodiscard]] constexpr auto iterations() const -> std::size_t
  {
    return m_params.iterations;
  }
  [[nodiscard]] constexpr auto ants() const -> std::size_t
  {
    return m_params.ants;
  }
  [[nodiscard]] constexpr auto evaporation_rate() const -> Number
  {
    return m_params.evaporation_rate;
  }
  [[nodiscard]] constexpr auto pheromone_contribution() const -> Number
  {
    return m_params.pheromone_contribution;
  }
  [[nodiscard]] constexpr auto minimal_pheromone_level() const -> Number
  {
    return m_params.minimal_pheromone_level;
  }

 private:
  class Ant
  {
   public:
    explicit Ant(const Params& params, const std::size_t index)
        : m_params(params), m_index(index)
    {
    }

    [[nodiscard]] auto walk(const AntGraph& graph) const
        -> VS::Vector<std::size_t>
    {
      const auto number_of_nodes = graph.number_of_nodes();
      std::set<std::size_t> available_indices{};
      for (std::size_t i = 0; i < number_of_nodes; ++i)
      {
        available_indices.insert(i);
      }

      auto current_index = m_params.start_node_index;
      available_indices.erase(current_index);

      auto ant_path = VS::Vector<std::size_t>{current_index};
      ant_path.reserve(number_of_nodes);

      while (!available_indices.empty())
      {
        const auto next_node_index =
            choose_next_node(graph, available_indices, current_index);

#ifdef DEBUG_PRINT
        std::cout << "Available indices: ";
        for (auto index : available_indices)
        {
          std::cout << index << ' ';
        }
        std::cout << "\nnext_node_index: " << next_node_index << '\n';
#endif

        ant_path.push_back(next_node_index);
        available_indices.erase(next_node_index);
        current_index = next_node_index;

        // using namespace std::chrono_literals;
        // std::this_thread::sleep_for(70ms);
      }

      return ant_path;
    }

   private:
    const Params& m_params;
    const std::size_t m_index;

    [[nodiscard]] auto choose_next_node(
        const AntGraph& graph,
        const std::set<std::size_t>& available_indices,
        const std::size_t current_node_index) const -> std::size_t
    {
      auto probabilities =
          VS::Vector<Number>(graph.number_of_nodes(), Number{0});
      for (auto index : available_indices)
      {
        const auto& weight = graph.get_weight(current_node_index, index);

        if (weight)
        {
          const auto& [desirability, pheromone] = weight.value();
          probabilities.at(index) =
              std::pow(desirability, m_params.desirability_weight) *
              std::pow(pheromone, m_params.pheromone_weight);
        }
      }

#ifdef DEBUG_PRINT
      std::cout << "Probabilities: ";
      for (auto probability : probabilities)
      {
        std::cout << probability << ' ';
      }
      std::cout << '\n';
#endif

      std::mt19937 generator(m_params.seed + m_index);
      std::discrete_distribution<std::size_t> distribution(
          probabilities.begin(), probabilities.end());
      return distribution(generator);
    }

    // [[nodiscard]] auto calculate_probabilities() const -> VS::Vector<Number>
    // {}
  };

  [[nodiscard]] constexpr auto build_ant_graph(
      const DirectedGraph<Weight>& graph) const -> AntGraph
  {
    const auto number_of_nodes = graph.number_of_nodes();

    // FIXME: add switch-case to use different priority rules
    // NOTE: this is an LRT rule
    const auto calculate_desirability = [](Number weight) -> Number
    {
      return Number{1} / weight;
    };

    auto ant_graph = AntGraph(number_of_nodes);
    for (VS::Index from = 0; from < number_of_nodes; ++from)
    {
      for (VS::Index to = 0; to < number_of_nodes; ++to)
      {
        const auto weight = graph.get_weight(from, to);

        if (weight)
        {
          ant_graph.get_weight(from, to) = {
              calculate_desirability(weight.value()),
              m_params.initial_pheromone_level};
        }
      }
    }

    return ant_graph;
  }

  constexpr void update_pheromones(
      AntGraph& graph,
      const VS::Vector<VS::Vector<Number>>& contributions) const
  {
    const auto number_of_nodes = graph.number_of_nodes();

    for (VS::Index from = 0; from < number_of_nodes; ++from)
    {
      for (VS::Index to = 0; to < number_of_nodes; ++to)
      {
        auto& weight = graph.get_weight(from, to);
        if (weight)
        {
          const auto [desirability, old_level] = weight.value();

          auto level =
              contributions.at(from).at(to) + evaporation_rate() * old_level;
          weight = {desirability,
                    high_pass_filter(level, minimal_pheromone_level())};
        }
      }
    }
  }

  [[nodiscard]] constexpr static auto calculate_path_length(
      const Uni::DirectedGraph<Weight>& graph,
      const Path& path) -> PathLength
  {
    return Traits::calculate_path_length(graph, path);
  }

  [[nodiscard]] constexpr static auto find_best_path_index(
      const VS::Vector<PathLength>& lengths) -> VS::Index
  {
    return Traits::find_best_path_index(lengths);
  }

  Params m_params;
};

}  // namespace Uni
