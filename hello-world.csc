import csjvm

var vm = csjvm.create_JavaVM({"-Djava.class.path=."})

var clazz = vm.find_class("HelloWorld")

var method = vm.get_static_method(clazz, "main", "([Ljava/lang/String;)V")

vm.call_static_void_method(clazz, method, {})

vm.destroy_JavaVM()
