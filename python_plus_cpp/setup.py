from distutils.core import setup, Extension

delser = Extension("delser",
                   sources=["delser.cpp"],
                   extra_compile_args=['--std=c++11'])


setup(name="delser",
      version='1.0',
      description="",
      ext_modules=[
#foo,
delser
])
