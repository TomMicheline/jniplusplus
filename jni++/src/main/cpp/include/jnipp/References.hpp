//
// JniPPReferences.hpp
// jni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//


#pragma once

#include "jnipp/Utilities.hpp"

class JniLocalReferenceScope {
public:
	explicit JniLocalReferenceScope(int capacity) {
        if (jni_pp::env()->PushLocalFrame(capacity)) {
            jni_pp::log_print(jni_pp::LOG_ERROR, "PushLocalFrame failed, out of memory.  Punting.");
            return;
        }
        framePushed = true;
    }
	JniLocalReferenceScope() : JniLocalReferenceScope(16) {}

	~JniLocalReferenceScope() {
		(void) releaseLocalRefs(nullptr);
    }

    jobject releaseLocalRefs(jobject savedReference) {
		if (framePushed) {
			framePushed = false;
			return jni_pp::env()->PopLocalFrame(savedReference);
		}
		return savedReference;
	}

private:
    bool framePushed = false;
};

template <typename JniType>
class JniScopedGlobalReference {
public:
	explicit JniScopedGlobalReference(JniType localReference) {
		if (!localReference) {
			// Why try to wrap a null reference?  Probably some other JNI call failed and we didn't notice
			throw std::runtime_error("JniScopedGlobalReference was passed a null local reference");
		}
		globalReference = static_cast<JniType>(jni_pp::env()->NewGlobalRef(localReference));
		if (!globalReference) {
			// Out of memory... what can you do?
			throw std::runtime_error("Out of memory error occurred in NewGlobalRef");
		}

		// Now we can say only moved JniScopedGlobalReferences have null globalReferences
	}

	JniScopedGlobalReference(JniScopedGlobalReference&& other)  noexcept :
			globalReference(other.globalReference) {
		other.globalReference = nullptr;
	}

	virtual ~JniScopedGlobalReference() {
		if (globalReference != nullptr) {
			jni_pp::env()->DeleteGlobalRef(globalReference);
		}
	}

	JniType operator*() const {
		assertm(globalReference, "Dereference of JniScopedGlobalReference after it has been moved is illegal");
		return globalReference;
	}

private:
	JniType globalReference;
};