#include <LibGUI/Plot.hh>
#include <LibMath/Graph.hh>
#include <LibOptimization/AntColony.hh>
#include <LibOptimization/Schedule.hh>
#include <VS/Heap.hh>
#include <VS/Pair.hh>
#include <VS/Timeline.hh>
#include <VS/Types.hh>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <random>

using Path = VS::Indices;
using Time = std::size_t;

std::size_t optimum;
Time infinity = 0;

std::size_t seed = 0;

struct ScheduleTraits
{
  using Time = ::Time;

  struct Machine
  {
    std::string name;
  };

  struct Job
  {
    struct Operation
    {
      Time processing_time;
    };

    VS::Vector<Operation> operations;
  };
};

auto machines = VS::Vector<ScheduleTraits::Machine>{};
auto jobs = VS::Vector<ScheduleTraits::Job>{};
auto presedence_constraints = VS::Vector<VS::Indices>{};

auto index_to_operation(const VS::Index index)
    -> VS::Pair<VS::Index, VS::Index>
{
  const auto machine_index = (index - 1) % machines.size();
  const auto job_index = (index - 1) / machines.size();
  return {machine_index, job_index};
}

auto operation_to_index(const VS::Index machine_index,
                        const VS::Index job_index) -> VS::Index
{
  return job_index * machines.size() + machine_index + 1;
}

[[nodiscard]] static auto path_to_schedule(const Path& path)
{
  auto schedule = Uni::Schedule<ScheduleTraits>(machines, jobs);
  for (const auto& index : path)
  {
    if (index == 0)
    {
      continue;
    }
    const auto& pair = index_to_operation(index);
    const auto machine_index = pair.first;
    const auto job_index = pair.second;
    schedule.schedule_job_for_machine(machine_index, job_index);
  }

  return schedule;
}

struct ACO_JSP_Traits
{
  using Weight = ::Time;
  using PathLength = ::Time;

  struct TimelineTraits
  {
    using Time = PathLength;
    using Task = VS::Index;
    using Subject = VS::Index;
  };

  using Number = long double;

  struct EdgeWeight
  {
    Number desirability;
    Number pheromone_level;
  };

  using AntGraph = Uni::DirectedGraph<EdgeWeight>;

  [[nodiscard]] static auto build_timeline(const Path& path)
  {
    // Convert path to schedule
    const auto schedule = path_to_schedule(path);
    return Uni::JobShopTraits<ScheduleTraits>::build_timeline(
        schedule, presedence_constraints);
  }

  // FIXME: remove NOLINT
  [[nodiscard]] static auto create_ant_path(
      const AntGraph& graph,
      // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
      const Number desirability_weight,
      const Number pheromone_weight) -> VS::Indices
  {
    VS::Indices path = {0};
    std::set<VS::Index> available_indices{};

    for (VS::Index job_index = 0; job_index < jobs.size(); ++job_index)
    {
      const auto& job_constraints = presedence_constraints.at(job_index);
      const auto first_machine_index = job_constraints.at(0);

      const auto graph_index =
          operation_to_index(first_machine_index, job_index);

      available_indices.insert(graph_index);
    }

    const auto get_random_available_index =
        [&](const VS::Index current_index) -> VS::Index
    {
      auto probabilities = VS::Vector<Number>{};
      probabilities.reserve(available_indices.size());

      Number total_sum{0};

      for (const auto& index : available_indices)
      {
        const auto optional_weight = graph.get_weight(current_index, index);

        if (optional_weight)
        {
          const auto& weight = optional_weight.value();
          const auto& [desirability, pheromone] = weight;

          const auto probability =
              std::pow(desirability, desirability_weight) *
              std::pow(pheromone, pheromone_weight);

          probabilities.push_back(probability);
          total_sum += probability;
          // std::cout << "desirability: " << desirability
          //           << ", pheromone: " << pheromone
          //           << "\nprobability: " << probabilities.back() << '\n';
          // probabilities.push_back(
          //     1.0 / (std::pow(desirability, desirability_weight) *
          //            std::pow(pheromone, pheromone_weight)));
        }
        else
        {
          throw std::runtime_error{
              "Unexpected empty std::optional encountered"};
        }
      }

      // Normalization
      for (auto& probability : probabilities)
      {
        probability /= total_sum;
      }

      // std::cout << " === Probabilities === \n";
      // for (const auto probability : probabilities)
      // {
      //   std::cout << probability << ' ';
      // }
      // std::cout << '\n';

      // std::random_device rd;
      static std::mt19937 generator(seed);
      std::discrete_distribution<VS::Index> distribution(probabilities.begin(),
                                                         probabilities.end());
      const auto offset = distribution(generator);

      auto it = available_indices.begin();
      for (VS::Index i = 0; i < offset; ++i)
      {
        ++it;
      }
      return *it;
    };

    VS::Index prev_index = 0;
    while (!available_indices.empty())
    {
      const auto next_index = get_random_available_index(prev_index);
      const auto [machine_index, job_index] = index_to_operation(next_index);

      const auto& job_constraints = presedence_constraints.at(job_index);
      for (VS::Index i = 0; i < machines.size(); ++i)
      {
        if (job_constraints.at(i) == machine_index)
        {
          if (i != machines.size() - 1)
          {
            const auto next_machine_index = job_constraints.at(i + 1);
            const auto graph_index =
                operation_to_index(next_machine_index, job_index);

            available_indices.insert(graph_index);
          }
        }
      }

      available_indices.erase(next_index);
      path.push_back(next_index);

      prev_index = next_index;
    }

    return path;
  }

  // FIXME: make this function constexpr
  static auto calculate_path_length(const Path& path) -> PathLength
  {
    const auto [timeline, is_deadlock] = build_timeline(path);

    if (is_deadlock)
    {
      // return infinity / (1 + timeline.get_number_of_events());
      std::cout << " === DEADLOCK ===\n" << timeline.to_ascii() << '\n';
      throw std::runtime_error{"DEADLOCK"};
    }

    return timeline.makespan();
  }

  constexpr static auto find_best_path_index(
      const VS::Vector<PathLength>& paths) -> VS::Index
  {
    return paths.reduce_to_index([](const auto& best, const auto& current)
                                 { return current < best; });
  }
};

[[nodiscard]] auto build_graph(const std::string& filename)
    -> Uni::DirectedGraph<Time>
{
  std::ifstream fin(filename);
  std::size_t number_of_jobs;
  std::size_t number_of_machines;
  fin >> number_of_jobs >> number_of_machines >> optimum;

  machines.reserve(number_of_machines);
  for (VS::Index i = 0; i < number_of_machines; ++i)
  {
    machines.push_back({std::to_string(i)});
  }

  jobs.reserve(number_of_jobs);
  for (VS::Index i = 0; i < number_of_jobs; ++i)
  {
    jobs.push_back(
        {VS::Vector<ScheduleTraits::Job::Operation>(number_of_machines, {})});
  }

  auto graph =
      Uni::DirectedGraph<Time>(number_of_machines * number_of_jobs + 1);

  auto times = VS::Vector<VS::Vector<Time>>(
      number_of_jobs, VS::Vector<Time>(number_of_machines, 0));

  for (VS::Index job = 0; job < number_of_jobs; ++job)
  {
    presedence_constraints.push_back(VS::Indices{});
    VS::Index prev = 0;
    for (VS::Index i = 0; i < number_of_machines; ++i)
    {
      VS::Index machine;
      Time operation_time;
      fin >> machine >> operation_time;

      times.at(job).at(machine) = operation_time;
      jobs.at(job).operations.at(i).processing_time = operation_time;

      infinity += operation_time;

      const VS::Index current = operation_to_index(machine, job);
      graph.get_weight(prev, current) = operation_time;

      presedence_constraints.at(job).push_back(machine);
      prev = current;
    }
  }

  infinity *= number_of_machines * number_of_jobs;

  for (VS::Index from_job = 0; from_job < number_of_jobs; ++from_job)
  {
    for (VS::Index from_machine = 0; from_machine < number_of_machines;
         ++from_machine)
    {
      const auto from_operation_time = times.at(from_job).at(from_machine);

      const auto start_index = 0;
      const auto from_index = operation_to_index(from_machine, from_job);
      graph.get_weight(start_index, from_index) = from_operation_time;

      for (VS::Index to_job = 0; to_job < number_of_jobs; ++to_job)
      {
        for (VS::Index to_machine = 0; to_machine < number_of_machines;
             ++to_machine)
        {
          if (from_machine == to_machine && from_job == to_job)
          {
            continue;
          }

          const auto to_operation_time = times.at(to_job).at(to_machine);
          const auto to_index = operation_to_index(to_machine, to_job);
          graph.get_weight(from_index, to_index) = to_operation_time;
        }
      }
    }
  }

  //   for (VS::Index machine = 0; machine < machines.size(); ++machine)
  //   {
  //     for (VS::Index i = 0; i < jobs.size(); ++i)
  //     {
  //       for (VS::Index j = i + 1; j < jobs.size(); ++j)
  //       {
  //         const VS::Index from = operation_to_index(machine, i);
  //         const VS::Index to = operation_to_index(machine, j);

  //         graph.get_weight(from, to) = times.at(i).at(machine);
  //         graph.get_weight(to, from) = times.at(j).at(machine);

  // #ifdef DEBUG_PRINT
  //         std::cout << "creating edge (" << to << ", " << from << ") \t("
  //                   << machine << ", " << j << ")\n";
  //         std::cout << "creating edge (" << from << ", " << to << ") \t("
  //                   << machine << ", " << i << ")\n";
  // #endif
  //       }
  //     }
  //   }

  return graph;
}

auto read_params(const std::string& config)
    -> Uni::AntColony<ACO_JSP_Traits>::Params
{
  auto params = Uni::AntColony<ACO_JSP_Traits>::Params{};

  std::ifstream fin(config);
  if (!fin)
  {
    throw std::runtime_error{"config was not found"};
  }

  // clang-format off
  fin >> params.seed

      >> params.pheromone_weight
      >> params.desirability_weight
      >> params.initial_pheromone_level
      >> params.pheromone_contribution
      >> params.minimal_pheromone_level
      >> params.evaporation_rate

      >> params.start_node_index

      >> params.ants
      >> params.moving_average_count
      >> params.stagnation_eps
      >> params.iterations;
  // clang-format on

  if (params.seed == 0)
  {
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dis(0,
                                                                 100'000'000);
    params.seed = dis(gen);
    std::cout << "SEED: " << params.seed << '\n';
  }

  seed = params.seed;

  return params;
}

// FIXME: remove NOLINT
// NOLINTNEXTLINE(bugprone-exception-escape)
auto main(int argc, const char* argv[]) -> int
{
  if (argc != 3)
  {
    std::cerr << "usage: " << argv[0] << " <config> <filename>\n";
    return 1;
  }

  const auto config = std::string{argv[1]};
  const auto filename = std::string{argv[2]};

  const auto graph = build_graph(filename);
  const auto params = read_params(config);

  auto aco = Uni::AntColony<ACO_JSP_Traits>{params};

  // struct Result
  // {
  //   struct
  //   {
  //     Path path;
  //     PathLength length;
  //   } best;
  //   VS::Vector<Path> best_paths;
  //   VS::Vector<PathLength> path_lengths;
  //   VS::Vector<ControlData> controls;
  //   VS::Vector<PathLength> moving_averages;
  // };
  // const std::size_t N = 100;

  // using Result = Uni::AntColony<ACO_JSP_Traits>::Result;

  // VS::Vector<Result> results;
  // for (VS::Index iteration = 0; iteration < N; ++iteration)
  // {
  //   const auto result = aco.run(graph);
  //   results.push_back(result);
  // }

  // VS::Vector<long double> upper_bound(params.iterations, 0);
  // VS::Vector<long double> mean_values(params.iterations, 0);
  // VS::Vector<long double> best_values(params.iterations, 0);
  // VS::Vector<long double> lower_bound(params.iterations, 0);

  // for (VS::Index i = 0; i < params.iterations; ++i)
  // {
  //   const auto lengths = results.template map<long double>(
  //       [i](const auto& result) { return result.controls.at(i).min; });

  //   const auto avg = Uni::mean(lengths);

  //   long double diff = 0;
  //   for (const auto& length : lengths)
  //   {
  //     diff += (avg - length) * (avg - length);
  //   }
  //   diff /= results.size();

  //   const auto stddev = std::sqrt(diff);
  //   const long double Z = 1.96;

  //   const long double error = Z * stddev / std::sqrt(N);

  //   upper_bound.at(i) = avg + error;
  //   mean_values.at(i) = avg;
  //   lower_bound.at(i) = avg - error;
  // }

  try
  {
    std::cout << "Running simulations...\n";
    const auto start_time = std::chrono::high_resolution_clock::now();
    auto result = aco.run(graph, true);
    const std::chrono::duration<double, std::milli> total_time =
        std::chrono::high_resolution_clock::now() - start_time;
    std::cout << total_time.count() << " ms elapsed\n";

    const auto& [path, length] = result.best;
    for (const auto& element : path)
    {
      std::cout << element << ' ';
    }

    const auto [best_timeline, _] = ACO_JSP_Traits::build_timeline(path);

    std::cout << "\nBest path (length: " << length << ", optimum: " << optimum
              << ")\n"
              << best_timeline.to_ascii() << '\n';

    const auto lengths = result.path_lengths;

    auto mins = VS::Vector<Time>{};
    auto means = VS::Vector<Time>{};
    auto maxs = VS::Vector<Time>{};
    auto iters = VS::Vector<Time>{};

    VS::Index iteration = 0;
    for (const auto& control : result.controls)
    {
      mins.push_back(control.min);
      means.push_back(control.mean);
      maxs.push_back(control.max);

      iters.push_back(iteration);

      iteration += 1;
    }

    Uni::Gui::plot_init();
    Uni::Gui::subplot(2, 1, 1);
    Uni::Gui::named_plot("min", iters.get_std(), mins.get_std());
    Uni::Gui::named_plot("mean", iters.get_std(), means.get_std());
    Uni::Gui::named_plot("max", iters.get_std(), maxs.get_std());
    Uni::Gui::named_plot(
        "moving average", iters.get_std(), result.moving_averages.get_std());
    Uni::Gui::legend();
    Uni::Gui::subplot(2, 1, 2);
    Uni::Gui::named_plot("min", iters.get_std(), mins.get_std());
    Uni::Gui::legend();
    Uni::Gui::show_plot();
  }
  catch (const std::runtime_error& error)
  {
    std::cout << "Error caught: " << error.what() << '\n';
  }

  return 0;
}
