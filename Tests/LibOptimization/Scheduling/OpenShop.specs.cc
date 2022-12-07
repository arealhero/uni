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
    ASSERT_EQ(lhs.at(i), rhs.at(i));
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
