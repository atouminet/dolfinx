// Copyright (C) 2007-2014 Anders Logg
//
// This file is part of DOLFIN (https://www.fenicsproject.org)
//
// SPDX-License-Identifier:    LGPL-3.0-or-later

#pragma once

#include "FormCoefficients.h"
#include "FormIntegrals.h"
#include <dolfin/common/types.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// Forward declaration
namespace ufc
{
class form;
}

namespace dolfin
{

namespace fem
{
class CoordinateMapping;
}

namespace function
{
class FunctionSpace;
}

namespace mesh
{
class Mesh;
template <typename T>
class MeshFunction;
} // namespace mesh

namespace fem
{

/// Base class for UFC code generated by FFC for DOLFIN with option -l.
///
/// A note on the order of trial and test spaces: FEniCS numbers
/// argument spaces starting with the leading dimension of the
/// corresponding tensor (matrix). In other words, the test space is
/// numbered 0 and the trial space is numbered 1. However, in order
/// to have a notation that agrees with most existing finite element
/// literature, in particular
///
///  \f[   a = a(u, v)        \f]
///
/// the spaces are numbered from right to left
///
///  \f[   a: V_1 \times V_0 \rightarrow \mathbb{R}  \f]
///
///
/// This is reflected in the ordering of the spaces that should be
/// supplied to generated subclasses. In particular, when a bilinear
/// form is initialized, it should be initialized as `a(V_1, V_0) = ...`,
/// where `V_1` is the trial space and `V_0` is the test space.
/// However, when a form is initialized by a list of argument spaces
/// (the variable `function_spaces` in the constructors below), the
/// list of spaces should start with space number 0 (the test space)
/// and then space number 1 (the trial space).

class Form
{
public:
  /// Create form (shared data)
  ///
  /// @param[in] ufc_form (ufc::form)
  ///         The UFC form.
  /// @param[in] function_spaces (std::vector<_function::FunctionSpace_>)
  ///         Vector of function spaces.
  Form(std::shared_ptr<const ufc::form> ufc_form,
       std::vector<std::shared_ptr<const function::FunctionSpace>>
           function_spaces);

  /// Destructor
  virtual ~Form();

  /// Return rank of form (bilinear form = 2, linear form = 1,
  /// functional = 0, etc)
  ///
  /// @return std::size_t
  ///         The rank of the form.
  std::size_t rank() const;

  /// Get the coefficient index for a named coefficient
  int get_coefficient_index(std::string name) const;

  /// Get the coefficient name for a given coefficient index
  std::string get_coefficient_name(int i) const;

  void set_coefficient_index_to_name_map(
      std::function<int(const char*)> coefficient_index_map);

  void set_coefficient_name_to_index_map(
      std::function<const char*(int)> coefficient_name_map);

  /// Return original coefficient position for each coefficient (0
  /// <= i < n)
  ///
  /// @return std::size_t
  ///         The position of coefficient i in original ufl form
  ///         coefficients.
  std::size_t original_coefficient_position(std::size_t i) const;

  /// Return the size of the element tensor, needed to create temporary space
  /// for assemblers. If the largest number of per-element dofs in
  /// function::FunctionSpace
  /// i is N_i, then for a linear form this is N_0, and for a bilinear form,
  /// N_0*N_1.
  ///
  /// @return std::size_t
  ///         The maximum number of values in a local element tensor
  ///
  /// FIXME: remove this, Assembler should calculate or put in utils
  std::size_t max_element_tensor_size() const;

  /// Set mesh, necessary for functionals when there are no function
  /// spaces
  ///
  /// @param[in] mesh (_mesh::Mesh_)
  ///         The mesh.
  void set_mesh(std::shared_ptr<const mesh::Mesh> mesh);

  /// Extract common mesh from form
  ///
  /// @return mesh::Mesh
  ///         Shared pointer to the mesh.
  std::shared_ptr<const mesh::Mesh> mesh() const;

  /// Return function space for given argument
  ///
  /// @param  i (std::size_t)
  ///         Index
  ///
  /// @return function::FunctionSpace
  ///         Function space shared pointer.
  std::shared_ptr<const function::FunctionSpace>
  function_space(std::size_t i) const;

  /// Return function spaces for arguments
  ///
  /// @return    std::vector<_function::FunctionSpace_>
  ///         Vector of function space shared pointers.
  std::vector<std::shared_ptr<const function::FunctionSpace>>
  function_spaces() const;

  /// Return cell domains (zero pointer if no domains have been
  /// specified)
  ///
  /// @return     _mesh::MeshFunction_ <std::size_t>
  ///         The cell domains.
  std::shared_ptr<const mesh::MeshFunction<std::size_t>> cell_domains() const;

  /// Return exterior facet domains (zero pointer if no domains have
  /// been specified)
  ///
  /// @return     std::shared_ptr<_mesh::MeshFunction_ <std::size_t>>
  ///         The exterior facet domains.
  std::shared_ptr<const mesh::MeshFunction<std::size_t>>
  exterior_facet_domains() const;

  /// Return interior facet domains (zero pointer if no domains have
  /// been specified)
  ///
  /// @return     _mesh::MeshFunction_ <std::size_t>
  ///         The interior facet domains.
  std::shared_ptr<const mesh::MeshFunction<std::size_t>>
  interior_facet_domains() const;

  /// Return vertex domains (zero pointer if no domains have been
  /// specified)
  ///
  /// @return     _mesh::MeshFunction_ <std::size_t>
  ///         The vertex domains.
  std::shared_ptr<const mesh::MeshFunction<std::size_t>> vertex_domains() const;

  /// Set cell domains
  ///
  /// @param[in]    cell_domains (_mesh::MeshFunction_ <std::size_t>)
  ///         The cell domains.
  void set_cell_domains(
      std::shared_ptr<const mesh::MeshFunction<std::size_t>> cell_domains);

  /// Set exterior facet domains
  ///
  ///  @param[in]   exterior_facet_domains (_mesh::MeshFunction_ <std::size_t>)
  ///         The exterior facet domains.
  void set_exterior_facet_domains(
      std::shared_ptr<const mesh::MeshFunction<std::size_t>>
          exterior_facet_domains);

  /// Set interior facet domains
  ///
  ///  @param[in]   interior_facet_domains (_mesh::MeshFunction_ <std::size_t>)
  ///         The interior facet domains.
  void set_interior_facet_domains(
      std::shared_ptr<const mesh::MeshFunction<std::size_t>>
          interior_facet_domains);

  /// Set vertex domains
  ///
  ///  @param[in]   vertex_domains (_mesh::MeshFunction_ <std::size_t>)
  ///         The vertex domains.
  void set_vertex_domains(
      std::shared_ptr<const mesh::MeshFunction<std::size_t>> vertex_domains);

  /// Access coefficients (non-const)
  FormCoefficients& coeffs() { return _coefficents; }

  /// Access coefficients (const)
  const FormCoefficients& coeffs() const { return _coefficents; }

  /// Access form integrals (const)
  const FormIntegrals& integrals() const { return _integrals; }

  /// Get ufc::coordinate_mapping (experimental)
  std::shared_ptr<const fem::CoordinateMapping> coordinate_mapping() const
  {
    return _coord_mapping;
  }

private:
  // Integrals associated with the Form
  FormIntegrals _integrals;

  // Coefficients associated with the Form
  FormCoefficients _coefficents;

  // Function spaces (one for each argument)
  std::vector<std::shared_ptr<const function::FunctionSpace>> _function_spaces;

  // The mesh (needed for functionals when we don't have any spaces)
  std::shared_ptr<const mesh::Mesh> _mesh;

  // Domain markers for cells
  std::shared_ptr<const mesh::MeshFunction<std::size_t>> dx;

  // Domain markers for exterior facets
  std::shared_ptr<const mesh::MeshFunction<std::size_t>> ds;

  // Domain markers for interior facets
  std::shared_ptr<const mesh::MeshFunction<std::size_t>> dS;

  // Domain markers for vertices
  std::shared_ptr<const mesh::MeshFunction<std::size_t>> dP;

  // ufc::coordinate_mapping
  std::shared_ptr<fem::CoordinateMapping> _coord_mapping;

  std::function<int(const char*)> _coefficient_index_map;
  std::function<const char*(int)> _coefficient_name_map;
};
} // namespace fem
} // namespace dolfin
