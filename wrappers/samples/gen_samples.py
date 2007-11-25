# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import buildfiles.wrap as wrap


def generate(local_ns, global_ns):
    """
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    local_ns.include()


    CodeInject = local_ns.class_('CodeInject')

    # Return type only function
    CodeInject.add_declaration_code("""
  int injectedFunc( samples::CodeInject& code ){
      return code.getValue();
  }
    """);
    CodeInject.add_registration_code(
        'def( "injectedFunc", &::injectedFunc )', works_on_instance = True)
                                     
    # Return and set type function
    CodeInject.add_declaration_code("""
    int otherInjectedFunc(samples::CodeInject& code, int value) {
        return code.setValue(value);
    }
    """);
    CodeInject.add_registration_code(
        'def("otherInjectedFunc", (int (*)(samples::CodeInject&, int))(&::otherInjectedFunc))',
        works_on_instance = True)


    # This does not work, because its append with a ".bp..."
    # if it wasn't this would work perfectly
    #CodeInject.add_registration_code(
    #    'bp::implicitly_convertible< int, samples::CodeInject >()')

    wrap.add_needed_includes([CodeInject])
    wrap.set_implicit_conversions([CodeInject], False)

    print 'Member Functions'
    for func in CodeInject.member_functions():
        print func

    return ['include/SharedPtrVector.h']

