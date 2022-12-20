#pragma once

#include <LibMath/Graph.hh>
#include <VS/Heap.hh>
#include <VS/Timeline.hh>
#include <VS/Vector.hh>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <type_traits>

// #define DEBUG_PRINT

namespace Uni
{

template <typename T>
concept ScheduleTraits = requires
{
  typename T::Time;
  typename T::Job;
  typename T::Job::Operation;
  typename T::Machine;

  requires requires(typename T::Job j, typename T::Job::Operation o)
  {
    {
      j.operations
      } -> std::same_as<VS::Vector<typename T::Job::Operation>&>;
    {
      o.processing_time
      } -> std::same_as<typename T::Time&>;
  };
};

template <ScheduleTraits Traits>
class Schedule
{
 public:
  using Time = typename Traits::Time;
  using Job = typename Traits::Job;
  using Machine = typename Traits::Machine;

  constexpr static auto downtime = std::nullopt;

  struct TimelineTraits
  {
    using Time = Time;
    using Task = Job;
    using Subject = Machine;
  };

  constexpr Schedule(const VS::Vector<Machine>& machines,
                     const VS::Vector<Job>& jobs)
      : m_machines(machines), m_jobs(jobs)
  {
  }
  constexpr Schedule(const VS::Vector<Machine>&& machines,
                     const VS::Vector<Job>& jobs)
      : m_machines(std::move(machines)), m_jobs(jobs)
  {
  }
  constexpr Schedule(const VS::Vector<Machine>& machines,
                     const VS::Vector<Job>&& jobs)
      : m_machines(machines), m_jobs(std::move(jobs))
  {
  }
  constexpr Schedule(const VS::Vector<Machine>&& machines,
                     const VS::Vector<Job>&& jobs)
      : m_machines(std::move(machines)), m_jobs(std::move(jobs))
  {
  }

  [[nodiscard]] constexpr auto get_schedule() const { return m_schedule; }

  [[nodiscard]] constexpr auto machines() const { return m_machines; }
  [[nodiscard]] constexpr auto jobs() const { return m_jobs; }

  [[nodiscard]] constexpr auto number_of_machines() const -> std::size_t
  {
    return machines().size();
  }

  [[nodiscard]] constexpr auto number_of_jobs() const -> std::size_t
  {
    return jobs().size();
  }

  constexpr void set_schedule_for_machine(const VS::Index machine_index,
                                          const VS::Indices& job_indices)
  {
    m_schedule[machine_index] = job_indices;
  }

  constexpr void schedule_job_for_machine(const VS::Index machine_index,
                                          const VS::Index job_index)
  {
    m_schedule[machine_index].push_back(job_index);
  }

  [[nodiscard]] constexpr auto get_schedule_for_machine(
      const VS::Index machine_index) const

  {
    return m_schedule.at(machine_index);
  }

  [[nodiscard]] constexpr auto at(const VS::Index machine_index) const
      -> const VS::Indices&

  {
    return m_schedule.at(machine_index);
  }
  [[nodiscard]] constexpr auto at(const VS::Index machine_index)
      -> VS::Indices&

  {
    return m_schedule.at(machine_index);
  }

  [[nodiscard]] constexpr auto to_timeline() const
      -> VS::Timeline<TimelineTraits>
  {
    throw std::runtime_error{"Method not implemented."};
  }

  [[nodiscard]] constexpr auto begin() const { return m_schedule.begin(); }
  [[nodiscard]] constexpr auto begin() { return m_schedule.begin(); }

  [[nodiscard]] constexpr auto end() const { return m_schedule.end(); }
  [[nodiscard]] constexpr auto end() { return m_schedule.end(); }

 private:
  VS::Vector<Machine> m_machines;
  VS::Vector<Job> m_jobs;
  std::map<VS::Index, VS::Indices> m_schedule;
};

template <typename T>
concept ScheduleBuilderTraits = ScheduleTraits<typename T::Traits> &&
    requires(const VS::Vector<typename T::Machine>& machines,
             const VS::Vector<typename T::Job>& jobs)
{
  {
    T::make_schedule(machines, jobs)
    } -> std::convertible_to<Schedule<typename T::Traits>>;
};

template <ScheduleBuilderTraits BuilderTraits>
class ScheduleBuilder
{
 public:
  using Traits = typename BuilderTraits::Traits;

  using Time = typename Traits::Time;
  using Job = typename Traits::Job;
  using Machine = typename Traits::Machine;

  [[nodiscard]] auto make_schedule(const VS::Vector<Machine>& machines,
                                   const VS::Vector<Job>& jobs) const
      -> Schedule<Traits>
  {
    return BuilderTraits::make_schedule(machines, jobs);
  }
};

template <ScheduleTraits UnderlyingTraits>
struct OpenShopTraits
{
 public:
  using Traits = UnderlyingTraits;
  using Time = typename Traits::Time;
  using Job = typename Traits::Job;
  using Machine = typename Traits::Machine;

  constexpr static auto make_schedule(const VS::Vector<Machine>& machines,
                                      const VS::Vector<Job>& jobs)
      -> Schedule<Traits>
  {
    // FIXME: assert that for every job the number of operations equals the
    // number of machines

    if (machines.size() == 2)
    {
      return make_schedule_for_2_machines(machines, jobs);
    }

    throw std::runtime_error{"Algorithm not implemented."};
  }

 private:
  constexpr static auto make_schedule_for_2_machines(
      const VS::Vector<Machine>& machines,
      const VS::Vector<Job>& jobs) -> Schedule<Traits>
  {
    assert(machines.size() == 2);

    VS::Vector<std::size_t> first_type;
    VS::Vector<std::size_t> second_type;

    for (std::size_t i = 0; i < jobs.size(); ++i)
    {
      const auto& job = jobs.at(i);
      const auto& operations = job.operations;
      if (operations.at(0).processing_time <= operations.at(1).processing_time)
      {
        first_type.push_back(i);
      }
      else
      {
        second_type.push_back(i);
      }
    }

    const auto [first_type_index, first_type_value] =
        first_type
            .map<Time>(
                [&jobs](const std::size_t index) -> Time
                { return jobs.at(index).operations.at(0).processing_time; })
            .find_max_element();

    const auto [second_type_index, second_type_value] =
        second_type
            .map<Time>(
                [&jobs](const std::size_t index) -> Time
                { return jobs.at(index).operations.at(1).processing_time; })
            .find_max_element();

    const auto max_index = first_type_value > second_type_value
                               ? first_type.at(first_type_index)
                               : second_type.at(second_type_index);

    const auto filter_max_index = [max_index](const std::size_t index) -> bool
    {
      return index != max_index;
    };

    const auto first_type_case = [&]()
    {
      auto schedule = Schedule<Traits>(machines, jobs);
      auto filtered_first_type = first_type.filter(filter_max_index);

      auto first_indices = VS::Vector<std::size_t>::merge(
          filtered_first_type, second_type, max_index);
      auto second_indices = VS::Vector<std::size_t>::merge(
          max_index, filtered_first_type, second_type);

      schedule.set_schedule_for_machine(0, first_indices);
      schedule.set_schedule_for_machine(1, second_indices);

      return schedule;
    };
    const auto second_type_case = [&]()
    {
      auto schedule = Schedule<Traits>(machines, jobs);
      auto filtered_second_type = second_type.filter(filter_max_index);

      auto first_indices =
          VS::Vector<std::size_t>::merge(VS::Vector<std::size_t>{max_index},
                                         filtered_second_type,
                                         first_type);
      auto second_indices =
          VS::Vector<std::size_t>::merge(filtered_second_type,
                                         first_type,
                                         VS::Vector<std::size_t>{max_index});

      schedule.set_schedule_for_machine(0, first_indices);
      schedule.set_schedule_for_machine(1, second_indices);

      return schedule;
    };

    if (max_index == first_type.at(first_type_index))
    {
      return first_type_case();
    }

    return second_type_case();
  }
};

template <ScheduleTraits Traits>
using OpenShopBuilder = ScheduleBuilder<OpenShopTraits<Traits>>;

template <ScheduleTraits UnderlyingTraits>
struct JobShopTraits
{
 public:
  using Traits = UnderlyingTraits;
  using Time = typename Traits::Time;
  using Path = VS::Indices;

  using Job = typename Traits::Job;
  using Machine = typename Traits::Machine;

  [[nodiscard]] constexpr static auto make_schedule(
      const VS::Vector<Machine>& machines,
      const VS::Vector<Job>& jobs) -> Schedule<Traits>
  {
    // FIXME: assert that for every job the number of operations equals the
    // number of machines

    if (machines.size() == 2)
    {
      return johnsons_method(machines, jobs);
    }

    throw std::runtime_error{"Algorithm not implemented."};
  }

  struct TimelineTraits
  {
    using Time = Time;
    using Task = VS::Index;
    using Subject = VS::Index;
  };

  [[nodiscard]] static auto build_timeline(
      const Schedule<Traits>& schedule,
      const VS::Vector<VS::Indices>& presence_constraints)
      -> VS::Pair<VS::Timeline<TimelineTraits>, bool>
  {
    const auto& jobs = schedule.jobs();

    const auto number_of_machines = schedule.number_of_machines();
    const auto number_of_jobs = schedule.number_of_jobs();

    auto operation_to_index = [number_of_machines](
                                  const VS::Index machine_index,
                                  const VS::Index job_index) -> VS::Index
    {
      return job_index * number_of_machines + machine_index;
    };

    auto timeline = VS::Timeline<TimelineTraits>{};

    auto operation_advance = VS::Indices(number_of_jobs, 0);

    auto machine_schedule_advance =
        VS::Vector<VS::Index>(number_of_machines, 0);
    auto machine_current_position =
        VS::Vector<VS::Index>(number_of_machines, 0);
    auto machine_busy_for = VS::Vector<Time>(number_of_machines, 0);
    auto job_busy_for = VS::Vector<Time>(number_of_jobs, 0);

    enum class MachineStatus
    {
      Waiting,
      Skipping,
      Running,
      Done,
    };

    VS::Vector<MachineStatus> machine_statuses(number_of_machines,
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

      for (VS::Index i = 0; i < number_of_jobs; ++i)
      {
        std::cout << "order of execution for job " << i << ": ";
        print_vector(presence_constraints.at(i));
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

      for (std::size_t machine_index = 0; machine_index < number_of_machines;
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
        if (schedule_index >= number_of_jobs)
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

        const auto operation_index = operation_advance.at(wanted_job_index);
        const auto wanted_machine_index =
            presence_constraints.at(wanted_job_index).at(operation_index);
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

        const auto operation_time = jobs.at(wanted_job_index)
                                        .operations.at(operation_index)
                                        .processing_time;
        const auto next_moment = moment + operation_time;

#ifdef DEBUG_PRINT
        std::cout << "machine " << machine_index << " will take the job "
                  << wanted_job_index << " for the next " << operation_time
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

 private:
  [[nodiscard]] constexpr static auto johnsons_method(
      const VS::Vector<Machine>& machines,
      const VS::Vector<Job>& jobs) -> Schedule<Traits>
  {
    assert(machines.size() == 2);
    // FIXME: assert that every job has exactly 2 operations

    struct Helper
    {
      Time minimum_time;
      std::size_t operation_index;
      std::size_t job_index;
    };

    std::size_t current_index = 0;
    // FIXME: rename
    VS::Vector<Helper> minimums = jobs.template map<Helper>(
        [&current_index](const auto& job) -> Helper
        {
          const auto& first_time = job.operations.at(0).processing_time;
          const auto& second_time = job.operations.at(1).processing_time;

          if (first_time < second_time)
          {
            return {
                first_time,
                0,
                current_index++,
            };
          }
          return {
              second_time,
              1,
              current_index++,
          };
        });

    minimums.sort([](const auto& lhs, const auto& rhs) -> bool
                  { return lhs.minimum_time < rhs.minimum_time; });

    VS::Vector<std::size_t> job_indices(minimums.size(), {});
    std::size_t left = 0;
    std::size_t right = minimums.size() - 1;
    for (const auto& element : minimums)
    {
      if (element.operation_index == 0)
      {
        job_indices.at(left++) = element.job_index;
      }
      else
      {
        job_indices.at(right--) = element.job_index;
      }
    }

    auto schedule = Schedule<Traits>(machines, jobs);
    schedule.set_schedule_for_machine(0, job_indices);
    schedule.set_schedule_for_machine(1, job_indices);
    return schedule;
  }
};

template <ScheduleTraits Traits>
using JobShopBuilder = ScheduleBuilder<JobShopTraits<Traits>>;

}  // namespace Uni
