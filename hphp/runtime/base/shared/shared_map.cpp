/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010- Facebook, Inc. (http://www.facebook.com)         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include <runtime/base/type_conversions.h>
#include <runtime/base/shared/shared_map.h>
#include <runtime/base/array/array_iterator.h>
#include <runtime/base/array/array_init.h>
#include <runtime/base/runtime_option.h>
#include <runtime/base/runtime_error.h>

namespace HPHP {

IMPLEMENT_SMART_ALLOCATION_HOT(SharedMap);
///////////////////////////////////////////////////////////////////////////////
HOT_FUNC
CVarRef SharedMap::getValueRef(ssize_t pos) const {
  SharedVariant *sv = m_arr->getValue(pos);
  DataType t = sv->getType();
  if (!IS_REFCOUNTED_TYPE(t)) return sv->asCVarRef();
  if (LIKELY(m_localCache != nullptr)) {
    assert(unsigned(pos) < m_arr->arrCap());
    TypedValue* tv = &m_localCache[pos];
    if (tv->m_type != KindOfUninit) return tvAsCVarRef(tv);
  } else {
    static_assert(KindOfUninit == 0, "must be 0 since we use smart_calloc");
    unsigned cap = m_arr->arrCap();
    m_localCache = (TypedValue*) smart_calloc(cap, sizeof(TypedValue));
  }
  TypedValue* tv = &m_localCache[pos];
  tvAsVariant(tv) = sv->toLocal();
  assert(tv->m_type != KindOfUninit);
  return tvAsCVarRef(tv);
}

HOT_FUNC
SharedMap::~SharedMap() {
  if (m_localCache) {
    for (TypedValue* tv = m_localCache, *end = tv + m_arr->arrCap();
         tv < end; ++tv) {
      tvRefcountedDecRef(tv);
    }
    smart_free(m_localCache);
  }
  m_arr->decRef();
}

bool SharedMap::exists(const StringData* k) const {
  return m_arr->getIndex(k) != -1;
}

bool SharedMap::exists(int64_t k) const {
  return m_arr->getIndex(k) != -1;
}

ssize_t SharedMap::getIndex(int64_t k) const {
  return m_arr->getIndex(k);
}

ssize_t SharedMap::getIndex(const StringData* k) const {
  return m_arr->getIndex(k);
}

CVarRef SharedMap::get(const StringData* k, bool error /* = false */) const {
  int index = m_arr->getIndex(k);
  if (index == -1) {
    return error ? getNotFound(k) : null_variant;
  }
  return getValueRef(index);
}

CVarRef SharedMap::get(int64_t k, bool error /* = false */) const {
  int index = m_arr->getIndex(k);
  if (index == -1) {
    return error ? getNotFound(k) : null_variant;
  }
  return getValueRef(index);
}

ArrayData *SharedMap::lval(int64_t k, Variant *&ret, bool copy,
                           bool checkExist /* = false */) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->lval(k, ret, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::lval(StringData* k, Variant *&ret, bool copy,
                           bool checkExist /* = false */) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->lval(k, ret, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::lvalNew(Variant *&ret, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->lvalNew(ret, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::set(int64_t k, CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->set(k, v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::set(StringData* k, CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->set(k, v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::setRef(int64_t k, CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->setRef(k, v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::setRef(StringData* k, CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->setRef(k, v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::remove(int64_t k, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->remove(k, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::remove(const StringData* k, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->remove(k, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::copy() const {
  return escalate();
}

ArrayData *SharedMap::append(CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->append(v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::appendRef(CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->appendRef(v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::appendWithRef(CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->appendWithRef(v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::append(const ArrayData *elems, ArrayOp op, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->append(elems, op, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::prepend(CVarRef v, bool copy) {
  ArrayData *escalated = escalate();
  ArrayData *ee = escalated->prepend(v, false);
  if (ee) {
    escalated->release();
    return ee;
  }
  return escalated;
}

ArrayData *SharedMap::escalate() const {
  ArrayData *ret = nullptr;
  m_arr->loadElems(ret, *this);
  assert(!ret->isStatic());
  return ret;
}

TypedValue* SharedMap::nvGet(int64_t k) const {
  int index = m_arr->getIndex(k);
  if (index == -1) return nullptr;
  return (TypedValue*)&getValueRef(index);
}

TypedValue* SharedMap::nvGet(const StringData* key) const {
  int index = m_arr->getIndex(key);
  if (index == -1) return nullptr;
  return (TypedValue*)&getValueRef(index);
}

void SharedMap::nvGetKey(TypedValue* out, ssize_t pos) {
  Variant k = m_arr->getKey(pos);
  TypedValue* tv = k.asTypedValue();
  // copy w/out clobbering out->_count.
  out->m_type = tv->m_type;
  out->m_data.num = tv->m_data.num;
  if (tv->m_type != KindOfInt64) out->m_data.pstr->incRefCount();
}

TypedValue* SharedMap::nvGetValueRef(ssize_t pos) {
  return const_cast<TypedValue*>(SharedMap::getValueRef(pos).asTypedValue());
}

TypedValue* SharedMap::nvGetCell(int64_t k) const {
  int index = m_arr->getIndex(k);
  return index != -1 ? getValueRef(index).getTypedAccessor() :
         nvGetNotFound(k);
}

TypedValue* SharedMap::nvGetCell(const StringData* key) const {
  int index = m_arr->getIndex(key);
  return index != -1 ? getValueRef(index).getTypedAccessor() :
         nvGetNotFound(key);
}

ArrayData* SharedMap::escalateForSort() {
  ArrayData *ret = nullptr;
  m_arr->loadElems(ret, *this, true /* mapInit */);
  assert(!ret->isStatic());
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
}
