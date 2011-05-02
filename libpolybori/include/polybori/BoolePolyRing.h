// -*- c++ -*-
//*****************************************************************************
/** @file BoolePolyRing.h 
 *
 * @author Alexander Dreyer
 * @date 2006-03-06
 *
 * This file the class BoolePolyRing, where carries the definition of a
 * polynomial ring over Booleans.
 *
 * @par Copyright:
 *   (c) 2006 by The PolyBoRi Team
 *
**/
//*****************************************************************************

// load PolyBoRi settings
# include "polybori/pbori_defs.h"

// include basic decision diagram manager interface 
#include <polybori/ring/CCuddCore.h>
#include <polybori/errors/PBoRiError.h>
#include "polybori/common/CCheckedIdx.h"
#include <boost/intrusive_ptr.hpp>

#include <list>

#ifndef polybori_BoolePolyRing_h_
#define polybori_BoolePolyRing_h_


BEGIN_NAMESPACE_PBORI



/** @class BoolePolyRing
 * @brief This class reinterprets decicion diagram managers as Boolean
 * polynomial rings, adds an ordering and variable names.
 *
 **/
class BoolePolyRing: 
  public CTypes::orderenums_type, public CTypes::compenums_type, 
  public CTypes::auxtypes_type  {

  /// generic access to current type
  typedef BoolePolyRing self;

 public:
  /// define exponent type
  typedef class BooleExponent exp_type;

  /// set monomial type
  typedef class BooleMonomial monom_type;

  /// set variables type
  typedef class BooleVariable var_type;

  /// set decision diagram type
  typedef class BooleSet dd_type;

  /// set polynomial type
  typedef class BoolePolynomial poly_type;
  /// @name adopt global type definitions
  //@{
  typedef CTypes::ordercode_type ordercode_type;
  typedef CTypes::vartext_type vartext_type;
  //@}

  /// Check indices bevor using
  typedef CCheckedIdx checked_idx_type;

  /// Type of actual data
  typedef CCuddCore core_type;
  typedef core_type::const_varname_reference const_varname_reference;
  /// Smart pointer to core
  typedef boost::intrusive_ptr<core_type> core_ptr;

  /// Type for handling mterm orderings
  typedef core_type::order_type order_type;

  /// Smart pointer for handling mterm orderings
  typedef core_type::order_ptr order_ptr;

  /// Reference for handling mterm orderings
  typedef order_type& order_reference;

  typedef DdManager mgr_type;
  /// Explicitely mention ordercodes' enumeration
  using CTypes::orderenums_type::ordercodes;

  /// Type for block indices
  typedef std::vector<idx_type> block_idx_type;

  /// Type for block iterators
  typedef block_idx_type::const_iterator block_iterator;

protected:

  /// Support for shallow copy (clone)
  /// @note May generate invalid ring, hence @c protected 
  BoolePolyRing(const core_ptr& rhs):  p_core(rhs) {}

public:
  /// Default constructor
  BoolePolyRing();

  /// Constructor for @em nvars variables
  explicit BoolePolyRing(size_type nvars, 
                         ordercode_type order = lp,
                         bool_type make_active = true);

   /// Constructor for @em nvars variables (and given pointer to ordering)
  BoolePolyRing(size_type nvars, const order_ptr& order):
    p_core(new core_type(nvars, order)) {}

  /// Copy constructor (cheap)
  BoolePolyRing(const self& rhs):  p_core(rhs.p_core) {}

  /// Destructor
  ~BoolePolyRing() {}

  /// Get number of ring variables
  size_type nVariables() const { return p_core->m_mgr.nVariables(); }

  /// Get name of variable with index idx
  vartext_type getVariableName(checked_idx_type idx) const {
    return p_core->m_names[idx];
  }

  /// Set name of variable with index idx
  void setVariableName(checked_idx_type idx, vartext_type varname) {
    p_core->m_names.set(idx, varname);
  }

  /// Clears the function cache
  void clearCache() { p_core->m_mgr.cacheFlush(); }

  /// Print out statistics and settings for current ring to output stream
  ostream_type& print(ostream_type&) const;

  /// Get unique identifier for manager of *this
  hash_type hash() const { 
    return static_cast<hash_type>(reinterpret_cast<std::ptrdiff_t
                                  >(getManager())); 
  }

  /// Access ordering of *this
  order_reference ordering() const { return *(p_core->pOrder); }

  /// Get plain decision diagram manager
  mgr_type* getManager() const {  return p_core->m_mgr.getManager(); }

  /// Construct ring with similiar properties (deep copy)
  self clone() const {  return self(new core_type(*p_core)); }

  /// Make @c *this global
  void activate();

  /// Change order of current ring
  void changeOrdering(ordercode_type);

  /// Map polynomial to this ring, if possible
  poly_type coerce(const poly_type& rhs) const;

  /// Map monomial to this ring, if possible
  monom_type coerce(const monom_type& rhs) const;

  /// Map variable to this ring
  var_type coerce(const var_type& rhs) const;

  /// Access nvar-th ring variable as diagram
  dd_type variable(checked_idx_type nvar) const;

  /// Get empty decision diagram 
  dd_type zero() const; // inlined below

  /// Get decision diagram with all variables negated
  dd_type one() const; // inlined below

  /// Get constant one or zero
  dd_type constant(bool is_one) const; // inlined below

protected:
  /// Access to actual data (via ->)
  core_ptr core() const {return p_core;};

  /// Smart pointer to actual data
  core_ptr p_core;
};

/// Stream output operator
inline BoolePolyRing::ostream_type& 
operator<<(BoolePolyRing::ostream_type& os, const BoolePolyRing& ring) {
  return ring.print(os);
}

END_NAMESPACE_PBORI

#include "polybori/BooleSet.h"

BEGIN_NAMESPACE_PBORI

 /// Get empty decision diagram 
inline BoolePolyRing::dd_type BoolePolyRing::zero() const { return dd_type(*this, p_core->m_mgr.zddZero()); }

  /// Get decision diagram with all variables negated
  inline  BoolePolyRing::dd_type BoolePolyRing::one() const { return dd_type(*this, p_core->m_mgr.zddOne()); }


  /// Get constant one or zero
inline  BoolePolyRing::dd_type BoolePolyRing::constant(bool is_one) const { return (is_one? one(): zero()); }

inline  BoolePolyRing::dd_type BoolePolyRing::variable(checked_idx_type nvar) const {
    return dd_type(*this, p_core->m_mgr.getVar(nvar)); 
  }

END_NAMESPACE_PBORI

#endif // of #ifndef polybori_BoolePolyRing_h_