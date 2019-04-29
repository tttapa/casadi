/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef CASADI_QPALM_INTERFACE_HPP
#define CASADI_QPALM_INTERFACE_HPP

#include "casadi/core/conic_impl.hpp"
#include <casadi/interfaces/qpalm/casadi_conic_qpalm_export.h>

// QPALM header
extern "C" {
#include "qpalm.h" // NOLINT(build/include)
#include "constants.h"
}

/** \defgroup plugin_Conic_qpalm
    Interface to the QPALM Solver for quadratic programming
*/

/** \pluginsection{Conic,qpalm} */

/// \cond INTERNAL
namespace casadi {

  struct CASADI_CONIC_QPALM_EXPORT QpalmMemory : public ConicMemory {
    // Structures
    QPALMData data;
    QPALMWorkspace *work;

    cholmod_common c;

    cholmod_sparse A, Q;

    std::vector<int> A_colind, A_row;

    std::vector<int> Q_colind, Q_row;

    /// Constructor
    QpalmMemory();

    /// Destructor
    ~QpalmMemory();
  };

  /** \brief \pluginbrief{Conic,qpalm}

      @copydoc Conic_doc
      @copydoc plugin_Conic_qpalm

  */
  class CASADI_CONIC_QPALM_EXPORT QpalmInterface : public Conic {
  public:
    /** \brief  Create a new Solver */
    explicit QpalmInterface(const std::string& name,
                             const std::map<std::string, Sparsity>& st);

    /** \brief  Create a new QP Solver */
    static Conic* creator(const std::string& name,
                                     const std::map<std::string, Sparsity>& st) {
      return new QpalmInterface(name, st);
    }

    /** \brief  Destructor */
    ~QpalmInterface() override;

    // Get name of the plugin
    const char* plugin_name() const override { return "qpalm";}

    // Get name of the class
    std::string class_name() const override { return "QpalmInterface";}

    ///@{
    /** \brief const Options */
    static const Options options_;
    const Options& get_options() const override { return options_;}
    ///@}

    /** \brief  Initialize */
    void init(const Dict& opts) override;

    /** \brief Create memory block */
    void* alloc_mem() const override { return new QpalmMemory();}

    /** \brief Initalize memory block */
    int init_mem(void* mem) const override;

    /** \brief Free memory block */
    void free_mem(void *mem) const override { delete static_cast<QpalmMemory*>(mem);}

    /// Solve the QP
    int solve(const double** arg, double** res,
        casadi_int* iw, double* w, void* mem) const override;

    /// Can discrete variables be treated
    bool integer_support() const override { return true;}

    /// Can psd constraints be treated
    bool psd_support() const override { return true;}

    /// A documentation string
    static const std::string meta_doc;

    /// Get all statistics
    Dict get_stats(void* mem) const override;

    QPALMSettings settings_;

    bool warm_start_primal_, warm_start_dual_;

    /** \brief Generate code for the function body */
    void codegen_body(CodeGenerator& g) const override;

    /** \brief Codegen alloc_mem */
    void codegen_init_mem(CodeGenerator& g) const override;

    /** \brief Codegen free_mem */
    void codegen_free_mem(CodeGenerator& g) const override;

    /** \brief Thread-local memory object type */
    std::string codegen_mem_type() const override { return "QpalmMemory"; }

    /** \brief Generate code for the declarations of the C function */
    void codegen_declarations(CodeGenerator& g) const override;

    void serialize_body(SerializingStream &s) const override;

    /** \brief Deserialize with type disambiguation */
    static ProtoFunction* deserialize(DeserializingStream& s) { return new QpalmInterface(s); }

  protected:
     /** \brief Deserializing constructor */
    explicit QpalmInterface(DeserializingStream& e);
  };

} // namespace casadi

/// \endcond
#endif // CASADI_QPALM_INTERFACE_HPP
