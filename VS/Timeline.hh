#pragma once

#include <VS/Vector.hh>
#include <algorithm>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>

namespace VS
{

template <typename T>
concept TimelineTraits = requires
{
  typename T::Time;
  typename T::Task;
  typename T::Subject;
};

template <typename Time>
struct Timespan
{
  Time start;
  Time end;

  [[nodiscard]] constexpr auto contains(const Timespan& other) const -> bool
  {
    return start <= other.start && other.end <= end;
  }

  [[nodiscard]] constexpr auto overlaps_with(const Timespan& other) const
      -> bool
  {
    return other.start <= end && start <= other.end;
  }
};

template <TimelineTraits Traits>
class Timeline
{
 public:
  using Time = typename Traits::Time;
  using Task = typename Traits::Task;
  using Subject = typename Traits::Subject;

  constexpr static std::optional<Task> downtime = std::nullopt;

  struct Event
  {
    Timespan<Time> timespan;
    std::optional<Task> task;

    constexpr static auto make_downtime(const Timespan<Time>& timespan)
        -> Event
    {
      return Event{timespan, downtime};
    }
  };

  [[nodiscard]] constexpr auto makespan() const -> Time { return m_makespan; }

  constexpr void insert(const Subject& subject, const Event& event)
  {
    // 0. If subject does not exist, create it
    if (!m_events.contains(subject))
    {
      resize_to_fit(event.timespan);

      auto events = Vector<Event>{};
      events.push_back(Event{{Time{0}, event.timespan.start}, downtime});
      events.push_back(event);
      if (event.timespan.end < makespan())
      {
        events.push_back(Event{{event.timespan.end, makespan()}, downtime});
      }

      m_events.insert({subject, events});
      return;
    }

    resize_to_fit(event.timespan);

    // 1. Test if this timespan is free
    if (!is_free(subject, event.timespan))
    {
      throw std::runtime_error{"Cannot insert event: timespan is not free"};
    }

    // 2. Resize timeline to fit this timespan
    // FIXME: we can probably resize it prior to this moment
    resize_to_fit(event.timespan);

    // 3. Insert event
    auto& events = get_events_by(subject);

    // 3.1. Find the downtime event
    auto it = events.begin();
    while (it != events.end() && !(it->timespan.contains(event.timespan)))
    {
      ++it;
    }

    if (it == events.end())
    {
      throw std::runtime_error{"Cannot find suitable downtime event"};
    }

    auto existing_event = *it;
    const auto initial_timespan = existing_event.timespan;
    Vector<Event> events_to_insert{};

    // 3.2. Resize or remove initial downtime event
    if (initial_timespan.start < event.timespan.start)
    {
      existing_event.timespan.end = event.timespan.start;
      events_to_insert.push_back(event);
    }
    else
    {
      existing_event.task = event.task;
      existing_event.timespan.end = event.timespan.end;
    }

    // 3.3. Optionally insert downtime event afterwards
    if (initial_timespan.end > event.timespan.end)
    {
      events_to_insert.push_back(
          Event::make_downtime({event.timespan.end, initial_timespan.end}));
    }

    // 3.4. Commit changes
    events.insert(it, events_to_insert.begin(), events_to_insert.end());
  }

  [[nodiscard]] constexpr auto get_events_by(const Subject& subject)
      -> Vector<Event>&
  {
    return m_events.at(subject);
  }
  [[nodiscard]] constexpr auto get_events_by(const Subject& subject) const
      -> const Vector<Event>&
  {
    return m_events.at(subject);
  }

  void print() const
  {
    for (const auto& [subject, events] : m_events)
    {
      std::cout << subject << ": ";
      for (const auto& event : events)
      {
        const auto length = event.timespan.end - event.timespan.start + 1;
        if (event.task)
        {
          std::stringstream str;
          str << event.task.value();
          const auto event_name = str.str();
          // FIXME: this won't work if event_name.length() > 1
          for (VS::Index i = 0; i < length; ++i)
          {
            std::cout << event_name;
          }
        }
        else
        {
          std::cout << std::string(length, '.');
        }
        // if (event.task)
        // {
        //   std::stringstream str;
        //   str << event.task.value();
        //   const auto event_name = str.str();

        //   const auto diff = (length - event_name.size());
        //   const auto left_padding = diff / 2;
        //   const auto right_padding = diff / 2 + diff % 2;

        //   std::cout << '|' << std::string(left_padding, '.') << event_name
        //             << std::string(right_padding, '.');
        // }
        // else
        // {
        //   std::cout << '|' << std::string(length, '-');
        // }
      }
      std::cout << '\n';
    }
  }

 private:
  std::map<Subject, Vector<Event>> m_events = {};
  Time m_makespan = 0;

  constexpr void resize_to_fit(const Timespan<Time>& timespan)
  {
    const auto current_end = makespan();
    if (timespan.end < current_end)
    {
      return;
    }

    m_makespan = timespan.end;

    for (auto& [subject, events] : m_events)
    {
      auto& last_event = events.back();
      if (is_downtime(last_event.task))
      {
        last_event.timespan.end = timespan.end;
      }
      else
      {
        events.emplace_back(Event{{current_end, timespan.end}, downtime});
      }
    }
  }

  [[nodiscard]] constexpr auto is_free(const Subject& subject,
                                       const Timespan<Time>& timespan) const
      -> bool
  {
    if (timespan.start >= makespan())
    {
      return true;
    }

    const auto& events = get_events_by(subject);
    return std::ranges::any_of(events,
                               [&timespan](const auto& event) -> bool {
                                 return event.timespan.contains(timespan) &&
                                        is_downtime(event.task);
                               });
  }

  [[nodiscard]] constexpr static auto is_downtime(
      const std::optional<Task>& task) -> bool
  {
    return !task.has_value();
  }
};

}  // namespace VS
