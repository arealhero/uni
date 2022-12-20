#include <gtest/gtest.h>

#include <LibOptimization/Schedule.hh>
#include <VS/Vector.hh>
#include <string>

struct ScheduleTraits
{
  using Time = std::size_t;

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

using Time = ScheduleTraits::Time;
using Machine = ScheduleTraits::Machine;
using Job = ScheduleTraits::Job;

template <typename T>
void assert_equal(const VS::Vector<T>& lhs, const VS::Vector<T>& rhs)
{
  ASSERT_EQ(lhs.size(), rhs.size());
  for (std::size_t i = 0; i < lhs.size(); ++i)
  {
    ASSERT_EQ(lhs.at(i), rhs.at(i));
  }
}

TEST(OpenShop, FirstCase_SimpleExample)
{
  auto machines = VS::Vector<Machine>{
      {"Milling machine"},
      {"Lathe"},
  };

  auto jobs = VS::Vector<Job>{
      {VS::Vector<Job::Operation>{{3}, {10}}},
      {VS::Vector<Job::Operation>{{12}, {8}}},
      {VS::Vector<Job::Operation>{{7}, {9}}},
      {VS::Vector<Job::Operation>{{2}, {4}}},
      {VS::Vector<Job::Operation>{{15}, {9}}},
  };

  const auto open_shop_builder = Uni::OpenShopBuilder<ScheduleTraits>{};
  const auto schedule = open_shop_builder.make_schedule(machines, jobs);

  const auto first_schedule = schedule.get_schedule_for_machine(0);
  const auto second_schedule = schedule.get_schedule_for_machine(1);

  assert_equal(schedule.get_schedule_for_machine(0),
               VS::Vector<std::size_t>{4, 1, 0, 2, 3});
  assert_equal(schedule.get_schedule_for_machine(1),
               VS::Vector<std::size_t>{1, 0, 2, 3, 4});
}

TEST(OpenShop, SecondCase_SimpleExample)
{
  auto machines = VS::Vector<Machine>{
      {"Milling machine"},
      {"Lathe"},
  };

  auto jobs = VS::Vector<Job>{
      {VS::Vector<Job::Operation>{{3}, {10}}},
      {VS::Vector<Job::Operation>{{12}, {8}}},
      {VS::Vector<Job::Operation>{{10}, {11}}},
      {VS::Vector<Job::Operation>{{2}, {4}}},
      {VS::Vector<Job::Operation>{{15}, {9}}},
  };

  const auto open_shop_builder = Uni::OpenShopBuilder<ScheduleTraits>{};
  const auto schedule = open_shop_builder.make_schedule(machines, jobs);

  const auto first_schedule = schedule.get_schedule_for_machine(0);
  const auto second_schedule = schedule.get_schedule_for_machine(1);

  assert_equal(schedule.get_schedule_for_machine(0),
               VS::Vector<std::size_t>{0, 3, 1, 4, 2});
  assert_equal(schedule.get_schedule_for_machine(1),
               VS::Vector<std::size_t>{2, 0, 3, 1, 4});
}

TEST(JobShop, SimpleExample)
{
  auto machines = VS::Vector<Machine>{
      {"Milling machine"},
      {"Lathe"},
  };

  auto jobs = VS::Vector<Job>{
      {VS::Vector<Job::Operation>{{4}, {5}}},
      {VS::Vector<Job::Operation>{{12}, {1}}},
      {VS::Vector<Job::Operation>{{2}, {11}}},
      {VS::Vector<Job::Operation>{{6}, {5}}},
      {VS::Vector<Job::Operation>{{15}, {3}}},
  };

  const auto job_shop_builder = Uni::JobShopBuilder<ScheduleTraits>{};
  const auto schedule = job_shop_builder.make_schedule(machines, jobs);

  const auto first_schedule = schedule.get_schedule_for_machine(0);
  const auto second_schedule = schedule.get_schedule_for_machine(1);

  assert_equal(schedule.get_schedule_for_machine(0),
               VS::Vector<std::size_t>{2, 0, 3, 4, 1});
  assert_equal(schedule.get_schedule_for_machine(1),
               VS::Vector<std::size_t>{2, 0, 3, 4, 1});
}

TEST(JobShop, TimelineBuilding)
{
  const auto machines = VS::Vector<Machine>{
      {"Milling machine"},
      {"Lathe"},
  };

  const auto jobs = VS::Vector<Job>{
      {VS::Vector<Job::Operation>{{4}, {5}}},
      {VS::Vector<Job::Operation>{{12}, {1}}},
      {VS::Vector<Job::Operation>{{2}, {11}}},
      {VS::Vector<Job::Operation>{{6}, {5}}},
      {VS::Vector<Job::Operation>{{15}, {3}}},
  };

  const auto presence_constraints = VS::Vector<VS::Indices>{
      {0, 1},
      {0, 1},
      {0, 1},
      {0, 1},
      {0, 1},
  };

  const auto job_shop_builder = Uni::JobShopBuilder<ScheduleTraits>{};
  const auto schedule = job_shop_builder.make_schedule(machines, jobs);

  const auto& [timeline, is_deadlock] =
      Uni::JobShopTraits<ScheduleTraits>::build_timeline(schedule,
                                                         presence_constraints);

  const auto expected_ascii_timeline =
      std::string("0: ") + std::string(2, '2') + std::string(4, '0') +
      std::string(6, '3') + std::string(15, '4') + std::string(12, '1') +
      std::string(".\n1: ..") + std::string(11, '2') + std::string(5, '0') +
      std::string(5, '3') + std::string(4, '.') + std::string(3, '4') +
      std::string(9, '.') + std::string(1, '1') + std::string("\n");

  ASSERT_FALSE(is_deadlock);
  ASSERT_EQ(timeline.makespan(), 40);
  ASSERT_EQ(timeline.to_ascii(), expected_ascii_timeline);
}
