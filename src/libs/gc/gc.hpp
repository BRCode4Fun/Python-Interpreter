#pragma once

#include <unordered_set>
#include <vector>

#include "../value/py_object.hpp"

/*
 * Tracks PyObject pointers handed out by the interpreter so objects with ref_count == 0 can be
 * deleted (manual reference counting only).
 *
 * Two containers:
 * - vector: discovery order / compact sweep in free_unused.
 * - unordered_set: O(1) amortized membership on push_object (avoids linear std::find).
 *
 * alloc_count counts unique registrations since the last free_unused() sweep (reset there).
 * Interpreter::maybe_collect() runs free_unused() when over_registration_budget().
 */
class GarbageCollector {

public:
    static constexpr unsigned int k_collection_threshold = 4096;

    GarbageCollector() = default;
    ~GarbageCollector() { free_unused(); } /* final sweep; registry may already be empty after relinquish */

    void free_unused();
    void push_object(PyObject* value);

    /** Forget tracked pointers without deleting PyObjects (shutdown: Scope teardown drives refcount). */
    void relinquish_all_registrations();

    /** True when enough unique registrations warrant a sweep at a safe interpreter boundary. */
    bool over_registration_budget() const { return alloc_count >= k_collection_threshold; }

private:
    std::vector<PyObject*> objects;
    std::unordered_set<PyObject*> registered;
    /* Unique pushes since the last sweep */
    unsigned int alloc_count = 0;
};
