#include <LibGUI/Plot.hh>
#include <LibMath/Graph.hh>
#include <LibOptimization/AntColony.hh>
#include <VS/Heap.hh>
#include <VS/Pair.hh>
#include <VS/Timeline.hh>
#include <VS/Types.hh>
#include <fstream>
#include <iostream>
#include <map>

using Path = VS::Indices;
using Time = std::size_t;

// #define DEBUG_PRINT

std::size_t jobs;
std::size_t machines;
std::size_t optimum;

constexpr Time infinity = 1'000;

auto operation_order = VS::Vector<VS::Indices>{};

auto index_to_operation(const VS::Index index)
    -> VS::Pair<VS::Index, VS::Index>
{
  const auto machine_index = index % machines;
  const auto job_index = index / machines;
  return {machine_index, job_index};
}

auto operation_to_index(const VS::Index machine_index,
                        const VS::Index job_index) -> VS::Index
{
  return job_index * machines + machine_index;
}

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

  static auto build_timeline(const Uni::DirectedGraph<Time>& graph,
                             const Path& path)
      -> VS::Pair<VS::Timeline<TimelineTraits>, bool>
  {
    auto schedule = std::map<std::size_t, Path>{};
    for (const auto& index : path)
    {
      if (index == 0)
      {
        continue;
      }
      const auto& pair = index_to_operation(index - 1);
      const auto machine_index = pair.first;
      const auto job_index = pair.second;
      schedule[machine_index].push_back(job_index);
    }

    auto timeline = VS::Timeline<TimelineTraits>{};

    auto operation_advance = VS::Indices(jobs, 0);

    auto machine_schedule_advance = VS::Vector<VS::Index>(machines, 0);
    auto machine_current_position = VS::Vector<VS::Index>(machines, 0);
    auto machine_busy_for = VS::Vector<Time>(machines, 0);
    auto job_busy_for = VS::Vector<Time>(jobs, 0);

    enum class MachineStatus
    {
      Waiting,
      Skipping,
      Running,
      Done,
    };

    VS::Vector<MachineStatus> machine_statuses(machines,
                                               MachineStatus::Waiting);

    VS::MinHeap<Time> moments{0};
    while (!moments.empty())
    {
      const auto moment = moments.pop();
      std::set<Time> moments_to_create{};

#ifdef DEBUG_PRINT
      const auto print_vector = [](const auto& vec)
      {
        for (const auto& element : vec)
        {
          std::cout << element << ' ';
        }
      };

      std::cout << "     =====     \n";
      std::cout << "moments: ";
      print_vector(moments);
      std::cout << '\n';

      for (VS::Index i = 0; i < jobs; ++i)
      {
        std::cout << "order of execution for job " << i << ": ";
        print_vector(operation_order.at(i));
        std::cout << '\n';
      }

      for (const auto& [machine_index, machine_schedule] : schedule)
      {
        std::cout << "schedule for machine " << machine_index << ": ";
        print_vector(machine_schedule);
        std::cout << '\n';
      }

      std::cout << "current moment: " << moment << '\n';
      std::cout << "operation_advance: ";
      print_vector(operation_advance);
      std::cout << "\nmachine_schedule_advance: ";
      print_vector(machine_schedule_advance);
      std::cout << "\nmachine_current_position: ";
      print_vector(machine_current_position);
      std::cout << '\n';
#endif

      for (std::size_t machine_index = 0; machine_index < machines;
           ++machine_index)
      {
        const auto machine_moment = machine_busy_for.at(machine_index);
        if (machine_moment > moment)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index
                    << " is busy (will be available at " << machine_moment
                    << ")\n";
#endif
          continue;
        }

        const auto schedule_index = machine_schedule_advance.at(machine_index);
        if (schedule_index >= jobs)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " is done\n";
#endif
          machine_statuses.at(machine_index) = MachineStatus::Done;
          continue;
        }

        const auto wanted_job_index =
            schedule.at(machine_index).at(schedule_index);
        const auto job_moment = job_busy_for.at(wanted_job_index);
        if (job_moment > moment)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " wanted job "
                    << wanted_job_index << ", but job will be available at "
                    << job_moment << " => waiting from " << moment << " to "
                    << job_moment << '\n';
#endif
          machine_statuses.at(machine_index) = MachineStatus::Waiting;
          timeline.insert(machine_index, {{moment, job_moment}, std::nullopt});
          machine_busy_for.at(machine_index) = job_moment;
          continue;
        }

        const auto job_index = operation_advance.at(wanted_job_index);
        const auto wanted_machine_index =
            operation_order.at(wanted_job_index).at(job_index);
        if (wanted_machine_index != machine_index)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " wanted job "
                    << wanted_job_index << ", but job needs machine "
                    << wanted_machine_index
                    << " => waiting for 1 unit of time\n";
#endif
          machine_statuses.at(machine_index) = MachineStatus::Skipping;
          timeline.insert(machine_index, {{moment, moment + 1}, std::nullopt});
          moments_to_create.insert(moment + 1);
          continue;
        }

        machine_schedule_advance.at(machine_index) += 1;

        auto& from = machine_current_position.at(machine_index);
        const auto to =
            operation_to_index(machine_index, wanted_job_index) + 1;

        const auto weight = graph.get_weight(from, to);
        if (weight)
        {
          const auto next_moment = moment + weight.value();

#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " will take the job "
                    << wanted_job_index << " for the next " << weight.value()
                    << " units of time\n";
#endif

          machine_busy_for.at(machine_index) = next_moment;
          operation_advance.at(wanted_job_index)++;
          job_busy_for.at(wanted_job_index) = next_moment;
          machine_statuses.at(machine_index) = MachineStatus::Running;

          timeline.insert(machine_index,
                          {{moment, next_moment}, wanted_job_index});

          from = to;

          moments_to_create.insert(next_moment);
        }
        else
        {
          throw std::runtime_error{"Something strange happened"};
        }
      }

      bool is_deadlock = true;
      bool all_done = true;
      for (auto status : machine_statuses)
      {
        if (status != MachineStatus::Done)
        {
          all_done = false;
        }

        if (status != MachineStatus::Skipping && status != MachineStatus::Done)
        {
          is_deadlock = false;
          break;
        }
      }

      if (is_deadlock && !all_done)
      {
        // calculate total_advance
        [[maybe_unused]] const auto total_advance =
            machine_schedule_advance.reduce(1, std::plus<>{});

#ifdef DEBUG_PRINT
        std::cout << "All machines are skipping => deadlock (total advance: "
                  << total_advance << ")\n";
#endif
        return {timeline, true};
      }

      for (auto moment : moments)
      {
        moments_to_create.insert(moment);
      }

      moments.clear();

      for (auto element : moments_to_create)
      {
        moments.push(element);
      }
    }

    return {timeline, false};
  }

  // FIXME: make this function constexpr
  static auto calculate_path_length(const Uni::DirectedGraph<Time>& graph,
                                    const Path& path) -> PathLength
  {
    auto schedule = std::map<std::size_t, Path>{};
    for (const auto& index : path)
    {
      if (index == 0)
      {
        continue;
      }
      const auto& pair = index_to_operation(index - 1);
      const auto machine_index = pair.first;
      const auto job_index = pair.second;
      schedule[machine_index].push_back(job_index);
    }

    auto timeline = VS::Timeline<TimelineTraits>{};

    auto operation_advance = VS::Indices(jobs, 0);

    auto machine_schedule_advance = VS::Vector<VS::Index>(machines, 0);
    auto machine_current_position = VS::Vector<VS::Index>(machines, 0);
    auto machine_busy_for = VS::Vector<Time>(machines, 0);
    auto job_busy_for = VS::Vector<Time>(jobs, 0);

    enum class MachineStatus
    {
      Waiting,
      Skipping,
      Running,
      Done,
    };

    VS::Vector<MachineStatus> machine_statuses(machines,
                                               MachineStatus::Waiting);

    VS::MinHeap<Time> moments{0};
    while (!moments.empty())
    {
      const auto moment = moments.pop();
      std::set<Time> moments_to_create{};

#ifdef DEBUG_PRINT
      const auto print_vector = [](const auto& vec)
      {
        for (const auto& element : vec)
        {
          std::cout << element << ' ';
        }
      };

      std::cout << "     =====     \n";
      std::cout << "moments: ";
      print_vector(moments);
      std::cout << '\n';

      for (VS::Index i = 0; i < jobs; ++i)
      {
        std::cout << "order of execution for job " << i << ": ";
        print_vector(operation_order.at(i));
        std::cout << '\n';
      }

      for (const auto& [machine_index, machine_schedule] : schedule)
      {
        std::cout << "schedule for machine " << machine_index << ": ";
        print_vector(machine_schedule);
        std::cout << '\n';
      }

      std::cout << "current moment: " << moment << '\n';
      std::cout << "operation_advance: ";
      print_vector(operation_advance);
      std::cout << "\nmachine_schedule_advance: ";
      print_vector(machine_schedule_advance);
      std::cout << "\nmachine_current_position: ";
      print_vector(machine_current_position);
      std::cout << '\n';
#endif

      for (std::size_t machine_index = 0; machine_index < machines;
           ++machine_index)
      {
        const auto machine_moment = machine_busy_for.at(machine_index);
        if (machine_moment > moment)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index
                    << " is busy (will be available at " << machine_moment
                    << ")\n";
#endif
          continue;
        }

        const auto schedule_index = machine_schedule_advance.at(machine_index);
        if (schedule_index >= jobs)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " is done\n";
#endif
          machine_statuses.at(machine_index) = MachineStatus::Done;
          continue;
        }

        const auto wanted_job_index =
            schedule.at(machine_index).at(schedule_index);
        const auto job_moment = job_busy_for.at(wanted_job_index);
        if (job_moment > moment)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " wanted job "
                    << wanted_job_index << ", but job will be available at "
                    << job_moment << " => waiting from " << moment << " to "
                    << job_moment << '\n';
#endif
          machine_statuses.at(machine_index) = MachineStatus::Waiting;
          timeline.insert(machine_index, {{moment, job_moment}, std::nullopt});
          machine_busy_for.at(machine_index) = job_moment;
          continue;
        }

        const auto job_index = operation_advance.at(wanted_job_index);
        const auto wanted_machine_index =
            operation_order.at(wanted_job_index).at(job_index);
        if (wanted_machine_index != machine_index)
        {
#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " wanted job "
                    << wanted_job_index << ", but job needs machine "
                    << wanted_machine_index
                    << " => waiting for 1 unit of time\n";
#endif
          machine_statuses.at(machine_index) = MachineStatus::Skipping;
          timeline.insert(machine_index, {{moment, moment + 1}, std::nullopt});
          moments_to_create.insert(moment + 1);
          continue;
        }

        machine_schedule_advance.at(machine_index) += 1;

        auto& from = machine_current_position.at(machine_index);
        const auto to =
            operation_to_index(machine_index, wanted_job_index) + 1;

        const auto weight = graph.get_weight(from, to);
        if (weight)
        {
          const auto next_moment = moment + weight.value();

#ifdef DEBUG_PRINT
          std::cout << "machine " << machine_index << " will take the job "
                    << wanted_job_index << " for the next " << weight.value()
                    << " units of time\n";
#endif

          machine_busy_for.at(machine_index) = next_moment;
          operation_advance.at(wanted_job_index)++;
          job_busy_for.at(wanted_job_index) = next_moment;
          machine_statuses.at(machine_index) = MachineStatus::Running;

          timeline.insert(machine_index,
                          {{moment, next_moment}, wanted_job_index});

          from = to;

          moments_to_create.insert(next_moment);
        }
        else
        {
          throw std::runtime_error{"Something strange happened"};
        }
      }

      bool is_deadlock = true;
      bool all_done = true;
      for (auto status : machine_statuses)
      {
        if (status != MachineStatus::Done)
        {
          all_done = false;
        }

        if (status != MachineStatus::Skipping && status != MachineStatus::Done)
        {
          is_deadlock = false;
          break;
        }
      }

      if (is_deadlock && !all_done)
      {
        // calculate total_advance
        const auto total_advance =
            machine_schedule_advance.reduce(1, std::plus<>{});

#ifdef DEBUG_PRINT
        std::cout << "All machines are skipping => deadlock (total advance: "
                  << total_advance << ")\n";
#endif
        return infinity / total_advance;
      }

      for (auto moment : moments)
      {
        moments_to_create.insert(moment);
      }

      moments.clear();

      for (auto element : moments_to_create)
      {
        moments.push(element);
      }
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
  fin >> jobs >> machines >> optimum;

  auto graph = Uni::DirectedGraph<Time>(machines * jobs + 1);

  auto times =
      VS::Vector<VS::Vector<Time>>(jobs, VS::Vector<Time>(machines, 0));

  for (VS::Index job = 0; job < jobs; ++job)
  {
    operation_order.push_back(VS::Indices{});
    VS::Index prev = 0;
    for (VS::Index i = 0; i < machines; ++i)
    {
      VS::Index machine;
      Time operation_time;
      fin >> machine >> operation_time;

      times.at(job).at(machine) = operation_time;

      const VS::Index current = operation_to_index(machine, job) + 1;
      graph.get_weight(prev, current) = operation_time;

      operation_order.at(job).push_back(machine);
      prev = current;
    }
  }

  for (VS::Index from_job = 0; from_job < jobs; ++from_job)
  {
    for (VS::Index from_machine = 0; from_machine < machines; ++from_machine)
    {
      const auto from_operation_time = times.at(from_job).at(from_machine);

      const auto start_index = 0;
      const auto from_index = operation_to_index(from_machine, from_job) + 1;
      graph.get_weight(start_index, from_index) = from_operation_time;

      for (VS::Index to_job = 0; to_job < jobs; ++to_job)
      {
        for (VS::Index to_machine = 0; to_machine < machines; ++to_machine)
        {
          if (from_machine == to_machine && from_job == to_job)
          {
            continue;
          }

          const auto to_operation_time = times.at(to_job).at(to_machine);
          const auto to_index = operation_to_index(to_machine, to_job) + 1;
          graph.get_weight(from_index, to_index) = to_operation_time;
        }
      }
    }
  }

  // for (VS::Index machine = 0; machine < machines; ++machine)
  // {
  //   for (VS::Index i = 0; i < jobs; ++i)
  //   {
  //     for (VS::Index j = i + 1; j < jobs; ++j)
  //     {
  //       const VS::Index from = operation_to_index(machine, i) + 1;
  //       const VS::Index to = operation_to_index(machine, j) + 1;

  //       std::cout << "creating edge (" << from << ", " << to << ") \t("
  //                 << machine << ", " << i << ")\n";
  //       graph.get_weight(from, to) = times.at(i).at(machine);

  //       std::cout << "creating edge (" << to << ", " << from << ") \t("
  //                 << machine << ", " << j << ")\n";
  //       graph.get_weight(to, from) = times.at(j).at(machine);
  //     }
  //   }
  // }

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
      >> params.iterations;
  // clang-format on

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

  try
  {
    auto nodes = graph.get_reachable_nodes(0);
    // std::cout << "Available nodes: ";
    // for (auto index : nodes)
    // {
    //   std::cout << index << ' ';
    // }
    // std::cout << '\n';

    std::cout << "Running simulations...\n";
    auto result = aco.run(graph);

    const auto& [path, length] = result.best;
    std::cout << "Best path (length: " << length << ", optimum: " << optimum
              << ")\n";
    for (const auto& element : path)
    {
      std::cout << element << ' ';
    }
    std::cout << '\n';

    // {
    //   auto schedule = std::map<std::size_t, Path>{};
    //   for (const auto& index : path)
    //   {
    //     if (index == 0)
    //     {
    //       continue;
    //     }
    //     const auto& pair = index_to_operation(index - 1);
    //     const auto machine_index = pair.first;
    //     const auto job_index = pair.second;
    //     schedule[machine_index].push_back(job_index);
    //   }

    //   for (const auto& [machine_index, machine_schedule] : schedule)
    //   {
    //     std::cout << "schedule for machine " << machine_index << ": ";
    //     for (const auto& index : machine_schedule)
    //     {
    //       std::cout << index << ' ';
    //     }
    //     std::cout << '\n';
    //   }
    // }

    auto iters = VS::Vector<Time>{};
    for (VS::Index i = 0; i < params.iterations; ++i)
    {
      iters.push_back(i);
    }

    // const auto last_control = result.controls.back();
    // std::cout << "Last control: min=" << last_control.min
    //           << ", mean=" << last_control.mean << ", max=" <<
    //           last_control.max
    //           << '\n';

    auto mins = VS::Vector<Time>{};
    auto means = VS::Vector<Time>{};
    auto maxs = VS::Vector<Time>{};
    for (const auto& control : result.controls)
    {
      mins.push_back(control.min);
      means.push_back(control.mean);
      maxs.push_back(control.max);
    }

    Uni::Gui::plot_init();
    Uni::Gui::named_plot("min", iters.get_std(), mins.get_std());
    Uni::Gui::named_plot("mean", iters.get_std(), means.get_std());
    Uni::Gui::named_plot("max", iters.get_std(), maxs.get_std());
    Uni::Gui::legend();
    Uni::Gui::show_plot();
  }
  catch (const std::runtime_error& error)
  {
    std::cout << "Error caught: " << error.what() << '\n';
  }

  return 0;
}
