#include "gc.hpp"

/*
 * Linear sweep over registered objects.
 *
 * Policy: any PyObject with ref_count == 0 may be destroyed. There is no tracing GC for cycles.
 *
 * Vector removal uses swap-with-last + pop_back:
 * - amortized O(1) per removal (vs erase in the middle, which shifts all trailing elements);
 * - keeps the vector dense without awkward iterator invalidation.
 *
 * After removing at index i, do not advance i: the slot now holds the former tail element and
 * must still be checked in this same pass.
 */
void GarbageCollector::free_unused() {
    size_t i = 0;
    while(i < objects.size()) {
        PyObject* object = objects[i];
        if(object->get_ref_count() == 0) {
            registered.erase(object);
            delete object;
            objects[i] = objects.back();
            objects.pop_back();
        } else {
            ++i;
        }
    }
    alloc_count = 0;
}

/*
 * Register each pointer at most once: unordered_set::insert reports false if it was already present.
 * Avoids duplicate vector entries and skewed counts without O(n) work per push.
 *
 * Sweep is not run here: registering the return value of a native call with rc==0 and immediately
 * freeing would delete the pointer still being returned to the caller. Call sites such as
 * Interpreter::maybe_collect() run free_unused() between statements or loop iterations.
 */
void GarbageCollector::push_object(PyObject* value) {
    if(value == nullptr) {
        return;
    }
    const auto inserted = registered.insert(value).second;
    if(!inserted) {
        return;
    }
    ++alloc_count;
    objects.push_back(value);
}

/* Call before tearing down builtins/globals — do not treat vector/registry as the live deletion list. */
void GarbageCollector::relinquish_all_registrations() {
    registered.clear();
    objects.clear();
    alloc_count = 0;
}