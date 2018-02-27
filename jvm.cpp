#include <covscript/cni.hpp>
#include <covscript/extension.hpp>
#include <jni.h>

static cs::extension jvm_ext;
static cs::extension_t jvm_ext_shared = cs::make_shared_extension(jvm_ext);

namespace jvm_cs_ext {
	using namespace cs;

	class jvm_instance final {
	public:
		JavaVM *jvm = nullptr;
		JNIEnv *env = nullptr;
		JavaVMInitArgs args;
		std::size_t options_count = 0;
		JavaVMOption *options = nullptr;
		jvm_instance() = delete;
		jvm_instance(const jvm_instance &) = delete;
		jvm_instance(jvm_instance &&) noexcept = delete;
		explicit jvm_instance(const array &optionsArray)
		{
			args.version = JNI_VERSION_1_8;
			options_count = optionsArray.size();
			args.nOptions = static_cast<jint>(options_count);
			options = new JavaVMOption[optionsArray.size()];
			for (std::size_t i = 0; i < options_count; ++i)
				options[i].optionString = strdup(optionsArray[i].to_string().c_str());
			args.options = options;
			args.ignoreUnrecognized = JNI_FALSE;
			if (JNI_CreateJavaVM(&jvm, (void **)&env, &args) != JNI_OK)
				throw lang_error("Create JVM Failed.");
		}
		~jvm_instance()
		{
			for (std::size_t i = 0; i < options_count; ++i)
				free(options[i].optionString);
			delete[] options;
		}
	};

	using jvm_holder = std::shared_ptr<jvm_instance>;

	jvm_holder create(const array &optionsArray)
	{
		return std::make_shared<jvm_instance>(optionsArray);
	}

	jclass find_class(const jvm_holder &holder, const cs::string &name)
	{
		return holder->env->FindClass(name.c_str());
	}

	jmethodID get_static_method(const jvm_holder &holder,
	                            const jclass clazz,
	                            const cs::string &name,
	                            const cs::string &signature)
	{
		if (clazz == nullptr)
			return nullptr;
		return holder->env->GetStaticMethodID(clazz, name.c_str(), signature.c_str());
	}

	void call_static_void_method(const jvm_holder &holder, jclass clazz,
	                             jmethodID method, array args)
	{
		if (clazz != nullptr && method != nullptr)
			holder->env->CallStaticVoidMethod(clazz, method);
	}

	void init()
	{
		jvm_ext.add_var("create", var::make_protect<callable>(cni(create), true));
		jvm_ext.add_var("find_class", var::make_protect<callable>(cni(find_class), true));
		jvm_ext.add_var("get_static_method", var::make_protect<callable>(cni(get_static_method), true));
		jvm_ext.add_var("call_static_void_method", var::make_protect<callable>(cni(call_static_void_method), true));
	}
}

namespace cs_impl {
	template <>
	cs::extension_t &get_ext<jvm_cs_ext::jvm_holder>()
	{
		return jvm_ext_shared;
	}

	template <>
	constexpr const char *get_name_of_type<jvm_cs_ext::jvm_holder>()
	{
		return "cs::jvm::JavaVM";
	}

	template <>
	constexpr const char *get_name_of_type<jclass>()
	{
		return "cs::jvm::JavaClass";
	}

	template <>
	constexpr const char *get_name_of_type<jmethodID>()
	{
		return "cs::jvm::JavaMethod";
	}

	template <>
	constexpr const char *get_name_of_type<jfieldID>()
	{
		return "cs::jvm::JavaField";
	}

	template <>
	constexpr const char *get_name_of_type<jobject>()
	{
		return "cs::jvm::JavaObject";
	}

	template <>
	constexpr const char *get_name_of_type<jthrowable>()
	{
		return "cs::jvm::JavaThrowable";
	}

	template <>
	constexpr const char *get_name_of_type<jstring>()
	{
		return "cs::jvm::JavaString";
	}

	template <>
	constexpr const char *get_name_of_type<jarray>()
	{
		return "cs::jvm::JavaArray";
	}

	template <>
	constexpr const char *get_name_of_type<jbooleanArray>()
	{
		return "cs::jvm::JavaBooleanArray";
	}

	template <>
	constexpr const char *get_name_of_type<jintArray>()
	{
		return "cs::jvm::JavaIntegerArray";
	}

	template <>
	constexpr const char *get_name_of_type<jfloatArray>()
	{
		return "cs::jvm::JavaFloatArray";
	}

	template <>
	constexpr const char *get_name_of_type<jdoubleArray>()
	{
		return "cs::jvm::JavaDoubleArray";
	}

	template <>
	constexpr const char *get_name_of_type<jcharArray>()
	{
		return "cs::jvm::JavaCharacterArray";
	}

	template <>
	constexpr const char *get_name_of_type<jbyteArray>()
	{
		return "cs::jvm::JavaByteArray";
	}

	template <>
	constexpr const char *get_name_of_type<jshortArray>()
	{
		return "cs::jvm::JavaShortArray";
	}

	template <>
	constexpr const char *get_name_of_type<jboolean>()
	{
		return "cs::jvm::JavaBoolean";
	}

	template <>
	constexpr const char *get_name_of_type<jint>()
	{
		return "cs::jvm::JavaInteger";
	}

	template <>
	constexpr const char *get_name_of_type<jfloat>()
	{
		return "cs::jvm::JavaFloat";
	}

	template <>
	constexpr const char *get_name_of_type<jdouble>()
	{
		return "cs::jvm::JavaDouble";
	}

	template <>
	constexpr const char *get_name_of_type<jchar>()
	{
		return "cs::jvm::JavaCharacter";
	}

	template <>
	constexpr const char *get_name_of_type<jbyte>()
	{
		return "cs::jvm::JavaByte";
	}

	template <>
	constexpr const char *get_name_of_type<jshort>()
	{
		return "cs::jvm::JavaShort";
	}
}

cs::extension *cs_extension()
{
	jvm_cs_ext::init();
	return &jvm_ext;
}
