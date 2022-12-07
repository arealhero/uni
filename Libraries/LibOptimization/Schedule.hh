#pragma once

#include <VS/Heap.hh>
#include <VS/Timeline.hh>
#include <VS/Vector.hh>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <type_traits>

namespace Uni
{

template <typename T>
concept ScheduleTraits = requires(T t,
                                  typename T::Job j,
                                  typename T::Job::Operation o)
{
  typename T::Time;
  typename T::Job;
  typename T::Job::Operation;
  typename T::Machine;

  {
    j.operations
    } -> std::same_as<VS::Vector<typename T::Job::Operation>&>;
  {
    o.processing_time
    } -> std::same_as<typename T::Time&>;
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

  constexpr void set_schedule_for_machine(
      const std::size_t machine_index,
      const VS::Vector<std::size_t>& job_indices)
  {
    m_schedule[machine_index] = job_indices;
  }

  constexpr VS::Vector<std::size_t> get_schedule_for_machine(
      const std::size_t machine_index) const

  {
    return m_schedule.at(machine_index);
  }

  constexpr VS::Timeline<TimelineTraits> to_timeline() const
  {
    throw std::runtime_error{"Method not implemented."};
  }

 private:
  VS::Vector<Machine> m_machines;
  VS::Vector<Job> m_jobs;
  std::map<std::size_t, VS::Vector<std::size_t>> m_schedule;
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

  Schedule<Traits> make_schedule(const VS::Vector<Machine>& machines,
                                 const VS::Vector<Job>& jobs) const
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

  constexpr static Schedule<Traits> make_schedule(
      const VS::Vector<Machine>& machines,
      const VS::Vector<Job>& jobs)
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
  constexpr static Schedule<Traits> make_schedule_for_2_machines(
      const VS::Vector<Machine>& machines,
      const VS::Vector<Job>& jobs)
  {
    assert(machines.size() == 2);

    VS::Vector<std::size_t> first_type;
    VS::Vector<std::size_t> second_type;

    for (std::size_t i = 0; i < jobs.size(); ++i)
    {
      const auto& job = jobs.at(i);
      const auto& operations = job.operations;
      if (operations.at(0).processing_time <= operations.at(1).processing_time)
        first_type.push_back(i);
      else
        second_type.push_back(i);
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
      return first_type_case();
    else
      return second_type_case();
  }
};

template <ScheduleTraits Traits>
using OpenShopBuilder = ScheduleBuilder<OpenShopTraits<Traits>>;

}  // namespace Uni
