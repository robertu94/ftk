#ifndef _FTK_CCA_UNION_FIND_H
#define _FTK_CCA_UNION_FIND_H

#include "ftk/basic/sparse_union_find.hh"

namespace ftk {

template <class IdType>
std::set<std::pair<IdType, IdType> > track_connected_components_uf(
    const std::vector<std::set<IdType> > &components0,
    const std::vector<std::set<IdType> > &components1) 
{
  auto overlaps = [](const std::set<IdType>& s0, const std::set<IdType>& s1) {
    for (const auto &e0 : s0) 
      if (s1.find(e0) != s1.end()) return true;
    return false;
  };

  std::set<std::pair<IdType, IdType> > results;

  for (size_t i = 0; i < components0.size(); i ++) {
    for (size_t j = 0; j < components1.size(); j ++) {
      if (overlaps(components0[i], components1[j])) {
        results.insert(std::make_pair(i, j));
      }
    }
  }

  return results;
}

template <class IdType, class ContainerType>
std::vector<std::set<IdType> > extract_connected_components_uf(
    const std::function<ContainerType(IdType) >& neighbors,
    const std::set<IdType> &qualified_)
{
  std::set<IdType> qualified(qualified_);

  sparse_union_find<IdType> UF; 
  for(auto ite = qualified.begin(); ite != qualified.end(); ++ite) {
    UF.add(*ite); 
  }

  for(auto ite = qualified.begin(); ite != qualified.end(); ++ite) {
    IdType current = *ite; 

    for (auto neighbor : neighbors(current)) {
      if (UF.has(neighbor)) {
        UF.unite(current, neighbor); 
      }
    }
  }  

  std::vector<std::set<IdType> > components = UF.get_sets();

  return components;
}

template <class IdType, class ContainerType>
std::vector<std::set<IdType> > extract_connected_components_uf(
    IdType nNodes,
    const std::function<ContainerType(IdType) >& neighbors,
    const std::function<bool(IdType)>& criterion)
{
  // fprintf(stderr, "finding qualified...\n");
  // find qualified
  std::set<IdType> qualified;
  for (IdType i=0; i<nNodes; i++) 
    if (criterion(i)) 
      qualified.insert(i);
  
  fprintf(stderr, "#qualified=%zu\n", qualified.size());

  return extract_connected_components(neighbors, qualified);
}

template <class IdType, class ContainerType>
std::vector<std::set<IdType> > extract_connected_components_uf(
    const ContainerType& nodes,
    const std::function<ContainerType(IdType) >& neighbors,
    const std::function<bool(IdType)>& criterion)
{
  std::set<IdType> qualified;
  for (const auto &n : nodes)
    if (criterion(n))
      qualified.insert(n);

  return extract_connected_components(neighbors, qualified);
}

}

#endif
