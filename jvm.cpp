#include <covscript/cni.hpp>
#include <covscript/extension.hpp>
#include <jni.h>

static cs::extension jvm_ext;
static cs::extension_t jvm_ext_shared = cs::make_shared_extension(jvm_ext);

namespace csjvm {
    using namespace cs;

    struct jvm_instance {
        JavaVM *jvm = nullptr;
        JNIEnv *env = nullptr;
        JavaVMInitArgs args;

        int options_count = 0;
        JavaVMOption *options = nullptr;
    };

    struct jvm_holder {
        jvm_instance *instance;
    };

    static void dealloc_instance(jvm_instance *instance) {
        for (int i = 0; i < instance->options_count; ++i) {
            free(instance->options[i].optionString);
        }
        delete[] instance->options;
        delete instance;
    }

    jvm_holder create_JavaVM(const array &optionsArray) {
        jvm_holder holder{nullptr};
        holder.instance = new jvm_instance;
        holder.instance->args.version = JNI_VERSION_1_8;
        holder.instance->options_count = static_cast<int>(optionsArray.size());
        holder.instance->args.nOptions = static_cast<jint>(holder.instance->options_count);

        holder.instance->options = new JavaVMOption[optionsArray.size()];
        for (int i = 0; i < holder.instance->options_count; ++i) {
            var optionString = optionsArray[i];
            holder.instance->options[i].optionString =
                    strdup(optionString.to_string().c_str());
        }
        holder.instance->args.options = holder.instance->options;
        holder.instance->args.ignoreUnrecognized = JNI_FALSE;

        JavaVM *javaVM;
        JNIEnv *env;
        jint result = JNI_CreateJavaVM(&javaVM, (void **) &env, &holder.instance->args);
        if (result == JNI_OK) {
            holder.instance->jvm = javaVM;
            holder.instance->env = env;
        } else {
            dealloc_instance(holder.instance);
            holder.instance = nullptr;
        }
        return holder;
    }

    void destroy_JavaVM(jvm_holder &holder) {
        if (holder.instance != nullptr) {
            dealloc_instance(holder.instance);
            holder.instance = nullptr;
        }
    }

    jclass find_class(const jvm_holder &holder, const cs::string &name) {
        if (holder.instance == nullptr) {
            return nullptr;
        }
        return holder.instance->env->FindClass(name.c_str());
    }

    jmethodID get_static_method(const jvm_holder &holder,
                                const jclass clazz,
                                const cs::string &name,
                                const cs::string &signature) {
        if (holder.instance == nullptr || clazz == nullptr) {
            return nullptr;
        }
        return holder.instance->env->GetStaticMethodID(clazz, name.c_str(), signature.c_str());
    }

    void call_static_void_method(const jvm_holder &holder, jclass clazz,
                                 jmethodID method, array args) {
        if (holder.instance == nullptr || clazz == nullptr || method == nullptr) {
            return;
        }
        holder.instance->env->CallStaticVoidMethod(clazz, method);
    }

    void init() {
        jvm_ext.add_var("create_JavaVM", var::make_protect<callable>(cni(create_JavaVM), true));
        jvm_ext.add_var("destroy_JavaVM", var::make_protect<callable>(cni(destroy_JavaVM), true));
        jvm_ext.add_var("find_class", var::make_protect<callable>(cni(find_class), true));
        jvm_ext.add_var("get_static_method", var::make_protect<callable>(cni(get_static_method), true));
        jvm_ext.add_var("call_static_void_method", var::make_protect<callable>(cni(call_static_void_method), true));
    }
}

namespace cs_impl {
    template<>
    cs::extension_t &get_ext<csjvm::jvm_holder>() {
        return jvm_ext_shared;
    }

    template<>
    constexpr const char *get_name_of_type<csjvm::jvm_holder>() {
        return "csjvm::JavaVM";
    }

    template<>
    constexpr const char *get_name_of_type<jclass>() {
        return "csjvm::JavaClass";
    }

    template<>
    constexpr const char *get_name_of_type<jmethodID>() {
        return "csjvm::JavaMethod";
    }

    template<>
    constexpr const char *get_name_of_type<jfieldID>() {
        return "csjvm::JavaField";
    }

    template<>
    constexpr const char *get_name_of_type<jobject>() {
        return "csjvm::JavaObject";
    }

    template<>
    constexpr const char *get_name_of_type<jthrowable>() {
        return "csjvm::JavaThrowable";
    }

    template<>
    constexpr const char *get_name_of_type<jstring>() {
        return "csjvm::JavaString";
    }

    template<>
    constexpr const char *get_name_of_type<jarray>() {
        return "csjvm::JavaArray";
    }

    template<>
    constexpr const char *get_name_of_type<jbooleanArray>() {
        return "csjvm::JavaBooleanArray";
    }

    template<>
    constexpr const char *get_name_of_type<jintArray>() {
        return "csjvm::JavaIntegerArray";
    }

    template<>
    constexpr const char *get_name_of_type<jfloatArray>() {
        return "csjvm::JavaFloatArray";
    }

    template<>
    constexpr const char *get_name_of_type<jdoubleArray>() {
        return "csjvm::JavaDoubleArray";
    }

    template<>
    constexpr const char *get_name_of_type<jcharArray>() {
        return "csjvm::JavaCharacterArray";
    }

    template<>
    constexpr const char *get_name_of_type<jbyteArray>() {
        return "csjvm::JavaByteArray";
    }

    template<>
    constexpr const char *get_name_of_type<jshortArray>() {
        return "csjvm::JavaShortArray";
    }

    template<>
    constexpr const char *get_name_of_type<jboolean>() {
        return "csjvm::JavaBoolean";
    }

    template<>
    constexpr const char *get_name_of_type<jint>() {
        return "csjvm::JavaInteger";
    }

    template<>
    constexpr const char *get_name_of_type<jfloat>() {
        return "csjvm::JavaFloat";
    }

    template<>
    constexpr const char *get_name_of_type<jdouble>() {
        return "csjvm::JavaDouble";
    }

    template<>
    constexpr const char *get_name_of_type<jchar>() {
        return "csjvm::JavaCharacter";
    }

    template<>
    constexpr const char *get_name_of_type<jbyte>() {
        return "csjvm::JavaByte";
    }

    template<>
    constexpr const char *get_name_of_type<jshort>() {
        return "csjvm::JavaShort";
    }
}

cs::extension *cs_extension() {
    csjvm::init();
    return &jvm_ext;
}
