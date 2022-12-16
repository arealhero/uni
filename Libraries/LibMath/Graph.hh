#pragma once

#include <VS/Pair.hh>
#include <VS/Vector.hh>
#include <optional>
#include <type_traits>

namespace Uni
{

template <typename G>
concept GraphTraits = requires()
{
  typename G::Weight;
  // typename G::Node;
  // // TODO: can we get rid of it?
  // std::is_default_constructible_v<typename G::Node>;

  {
    G::is_directed
    } -> std::convertible_to<bool>;
};

template <GraphTraits Traits>
class Graph
{
 public:
  constexpr static bool is_directed = Traits::is_directed;

  using Weight = typename Traits::Weight;
  using NodeEdges = VS::Vector<std::optional<Weight>>;

  struct Edge
  {
    VS::Index from;
    VS::Index to;
    Weight weight;
  };

  explicit constexpr Graph(const VS::Index size)
      : m_edges(size, NodeEdges(size, {}))
  {
  }

  [[nodiscard]] constexpr auto get_reachable_nodes(
      const VS::Index& node_index) const -> VS::Vector<VS::Index>
  {
    VS::Vector<VS::Index> result;
    for (VS::Index i = 0; i < m_edges.at(node_index).size(); ++i)
    {
      const auto& weight = m_edges.at(node_index).at(i);
      if (weight.has_value())
      {
        result.push_back(i);
      }
    }
    return result;
  }

  [[nodiscard]] constexpr auto get_edges(const VS::Index& node_index) const
  {
    return m_edges.at(node_index);
  }

  // [[nodiscard]] constexpr auto get_edge(const Index& from, const Index& to)
  //     -> Edge&
  // {
  //   return m_edges.at(from).at(to);
  // }

  [[nodiscard]] constexpr auto get_weight(const VS::Index& from,
                                          const VS::Index& to)
      -> std::optional<Weight>&
  {
    return m_edges.at(from).at(to);
  }
  [[nodiscard]] constexpr auto get_weight(const VS::Index& from,
                                          const VS::Index& to) const
      -> const std::optional<Weight>&
  {
    return m_edges.at(from).at(to);
  }

  // [[nodiscard]] constexpr auto get_edge(const Index& from,
  //                                       const Index& to) const -> const
  //                                       Edge&
  // {
  //   return m_edges.at(from).at(to);
  // }

  constexpr void insert(const Edge& edge)
  {
    m_edges.at(edge.from).at(edge.to) = edge.weight;
    if constexpr (!is_directed)
    {
      m_edges.at(edge.to).at(edge.from) = edge.weight;
    }
  }

  [[nodiscard]] constexpr auto number_of_nodes() const
  {
    return m_edges.size();
  }

 private:
  VS::Vector<NodeEdges> m_edges;
};

template <typename WeightType>
struct DirectedGraphTraits
{
  using Weight = WeightType;
  constexpr static bool is_directed = true;
};

template <typename Weight>
using DirectedGraph = Graph<DirectedGraphTraits<Weight>>;

}  // namespace Uni
