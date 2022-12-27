#pragma once

#include <LibMath/Filters.hh>
#include <LibMath/Graph.hh>
#include <LibStatistics/Numbers.hh>
#include <VS/Pair.hh>
#include <VS/Traits.hh>
#include <VS/Types.hh>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <set>
#include <thread>

namespace Uni
{

template <typename Traits>
concept AntColonyTraits = requires(const VS::Indices& path)
{
  typename Traits::Weight;
  typename Traits::PathLength;

  typename Traits::EdgeWeight;

  requires requires(typename Traits::EdgeWeight edge)
  {
    {
      edge.desirability
      } -> std::same_as<long double&>;

    {
      edge.pheromone_level
      } -> std::same_as<long double&>;
  };

  {
    Traits::calculate_path_length(path)
    } -> std::same_as<typename Traits::PathLength>;

  requires requires(const VS::Vector<typename Traits::PathLength>& lengths)
  {
    {
      Traits::find_best_path_index(lengths)
      } -> std::same_as<VS::Index>;
  };

  requires requires(const DirectedGraph<typename Traits::EdgeWeight>& graph,
                    const long double desirability_weight,
                    const long double pheromone_weight)
  {
    {
      Traits::create_ant_path(graph, desirability_weight, pheromone_weight)
      } -> std::same_as<VS::Indices>;
  };
};

template <AntColonyTraits Traits>
class AntColony
{
 public:
  // FIXME: get from Traits?
  using Number = long double;

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
    std::size_t moving_average_count;
    Number stagnation_eps;
    std::size_t iterations;
  };

  using EdgeWeight = typename Traits::EdgeWeight;
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
    VS::Vector<PathLength> moving_averages;
  };

  constexpr explicit AntColony(const Params& params) : m_params(params) {}

  // FIXME: think of a return type
  [[nodiscard]] constexpr auto run(const DirectedGraph<Weight>& graph,
                                   const bool use_iterations = false) const
      -> Result
  {
    assert(graph.number_of_nodes() > 1);
    auto ant_graph = build_ant_graph(graph);

    // Statistics collection
    auto path_lengths = VS::Vector<PathLength>{};
    auto iteration_controls = VS::Vector<ControlData>{};
    auto moving_averages = VS::Vector<PathLength>{};
    auto best_paths = VS::Vector<Path>{};

    const auto calculate_moving_average = [&]()
    {
      VS::Vector<ControlData> last_controls;

      if (moving_average_count() > iteration_controls.size())
      {
        last_controls = iteration_controls;
      }
      else
      {
        last_controls = iteration_controls.subvector(
            iteration_controls.size() - moving_average_count(),
            moving_average_count());
      }

      return mean(last_controls.template map<Number>(
          [](const auto& control) -> Number { return Number(control.min); }));
    };

    Number prev_moving_average = 0;
    Number moving_average_diff = 0;

    Path best{};
    PathLength best_path_length = 100'000'000;

    const auto iteration = [&](const VS::Index iteration)
    {
      std::cout << "Iteration " << iteration << '\n';

      // {
      //   const std::size_t column_width = 10;
      //   std::cout << " === Graph === \n";
      //   std::cout << std::setw(column_width) << ' ';
      //   for (VS::Index from = 0; from < ant_graph.number_of_nodes(); ++from)
      //   {
      //     std::cout << std::setw(column_width) << from << ' ';
      //   }
      //   std::cout << '\n';

      //   for (VS::Index from = 0; from < ant_graph.number_of_nodes(); ++from)
      //   {
      //     std::cout << std::setw(column_width) << from << ' ';
      //     for (VS::Index to = 0; to < ant_graph.number_of_nodes(); ++to)
      //     {
      //       const auto& weight = ant_graph.get_weight(from, to);
      //       if (weight)
      //       {
      //         std::cout << std::setw(column_width)
      //                   << weight.value().pheromone_level << ' ';
      //       }
      //       else
      //       {
      //         std::cout << std::setw(column_width) << "N/A" << ' ';
      //       }
      //     }
      //     std::cout << '\n';
      //   }
      // }

      // FIXME: change to `Matrix<Number>`?
      auto contributions = VS::Vector<VS::Vector<Number>>(
          graph.number_of_nodes(),
          VS::Vector<Number>(graph.number_of_nodes(), Number{0}));

      auto current_paths = VS::Vector<Path>{};
      auto current_lengths = VS::Vector<PathLength>{};
      auto paths_mutex = std::mutex{};

      current_paths.reserve(ants());
      current_lengths.reserve(ants());

      VS::Vector<std::thread> threads;
      // unsigned int number_of_threads = std::thread::hardware_concurrency();
      threads.reserve(ants());

      for (VS::Index ant_index = 0; ant_index < ants(); ++ant_index)
      {
        const auto run_ant = [&](const VS::Index ant_index) -> void
        {
          const auto ant = Ant{get_params(), ant_index};
          const auto path = ant.walk(ant_graph);
          const auto path_length = calculate_path_length(path);

          std::lock_guard<std::mutex> guard(paths_mutex);
          std::cout << "Ant " << ant_index << " path:\n";
          for (auto index : path)
          {
            std::cout << index << ' ';
          }
          std::cout << "\npath length = " << path_length << ", contribution = "
                    << (pheromone_contribution() / (long double)path_length)
                    << '\n';
          for (VS::Index j = 0; j < path.size() - 1; ++j)
          {
            const auto current_index = path.at(j);
            const auto next_index = path.at(j + 1);

            // FIXME: make this division optional (sometimes ACO should prefer
            // longer paths)
            contributions.at(current_index).at(next_index) +=
                pheromone_contribution() / (long double)path_length;
          }

          if (path_length < best_path_length)
          {
            best = path;
            best_path_length = path_length;
          }

          current_paths.push_back(path);
          path_lengths.push_back(path_length);
          current_lengths.push_back(path_length);
        };

        // FIXME: use optimal number of threads & give them tasks when needed
        threads.push_back(std::thread(run_ant, ant_index));
        // run_ant(ant_index);
      }

      for (auto& thread : threads)
      {
        thread.join();
      }

      // std::cout << "Threads joined\n";
      const auto best_path_index = find_best_path_index(current_lengths);
      best_paths.push_back(current_paths.at(best_path_index));

      iteration_controls.push_back(
          {min(current_lengths), mean(current_lengths), max(current_lengths)});

      update_pheromones(ant_graph, contributions);
      const auto current_moving_average = calculate_moving_average();
      moving_averages.push_back(current_moving_average);

      moving_average_diff = current_moving_average - prev_moving_average;

      std::cout << "Previous moving average: " << prev_moving_average
                << ", current moving average: " << current_moving_average
                << " (diff: " << moving_average_diff << ")\n";

      prev_moving_average = current_moving_average;
    };

    if (use_iterations)
    {
      for (VS::Index i = 0; i < m_params.iterations; ++i)
      {
        iteration(i);
      }
    }
    else
    {
      VS::Index i = 0;
      do
      {
        iteration(i);

        i += 1;
        if (i >= moving_average_count() &&
            std::abs(moving_average_diff) < stagnation_eps())
        {
          break;
        }

      } while (true);
    }

    return {{best, best_path_length},
            best_paths,
            path_lengths,
            iteration_controls,
            moving_averages};
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

  [[nodiscard]] constexpr auto moving_average_count() const -> std::size_t
  {
    return m_params.moving_average_count;
  }
  [[nodiscard]] constexpr auto stagnation_eps() const -> Number
  {
    return m_params.stagnation_eps;
  }

 private:
  class Ant
  {
   public:
    explicit Ant(const Params& params, const std::size_t index)
        : m_params(params), m_index(index)
    {
    }

    // FIXME: make this function generic (it now fits JSSP only)
    [[nodiscard]] auto walk(const AntGraph& graph) const -> VS::Indices
    {
      return create_ant_path(
          graph, m_params.desirability_weight, m_params.pheromone_weight);
    }

   private:
    const Params& m_params;
    const std::size_t m_index;
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
          level = std::max(level, minimal_pheromone_level());
          weight = {desirability, level};

          if (level != old_level)
          {
            std::cout << "Edge (" << from << ", " << to
                      << "): old level: " << old_level << ", new level: "
                      << std::max(level, minimal_pheromone_level()) << '\n';
          }
        }
      }
    }
  }

  [[nodiscard]] constexpr static auto create_ant_path(
      const AntGraph& graph,
      const Number desirability_weight,
      const Number pheromone_weight) -> VS::Indices
  {
    return Traits::create_ant_path(
        graph, desirability_weight, pheromone_weight);
  }

  [[nodiscard]] constexpr static auto calculate_path_length(const Path& path)
      -> PathLength
  {
    return Traits::calculate_path_length(path);
  }

  [[nodiscard]] constexpr static auto find_best_path_index(
      const VS::Vector<PathLength>& lengths) -> VS::Index
  {
    return Traits::find_best_path_index(lengths);
  }

  Params m_params;
};

}  // namespace Uni
