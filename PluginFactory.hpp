// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#pragma once


#include <memory>
#include <string> // string might not be allowed
#include <functional>
#include <map>

// necessary (because we're using <string>?)
extern "C" {
  __attribute__((weak))
  int __exidx_start(){ return -1;}
  __attribute__((weak))
  int __exidx_end(){ return -1; }
}

class Plugin;

// stolen from/inspired by
// https://www.codeproject.com/Articles/567242/AplusC-2b-2bplusObjectplusFactory

class PluginFactory {
public:
    static PluginFactory* Instance() {
        static PluginFactory factory;
        return &factory;
    }

    std::unique_ptr<Plugin>
    Create(std::string name) {
      std::unique_ptr<Plugin> instance;

      if (name.empty()) {
        Serial.println("Cannot instantiate a NullPlugin.");
        return instance;
        // name = "TestPlugin";
      }

      // find name in the registry and call factory method.
      auto it = factoryFunctionRegistry.find(name);
      if (it != factoryFunctionRegistry.end()) {
        instance = it->second();
      }

      // wrap instance in a shared ptr and return
      return instance;
    }
    void RegisterFactoryFunction(std::string name,
                                            std::function<std::unique_ptr<Plugin>(void)> classFactoryFunction) {
        // register the class factory function
        factoryFunctionRegistry[name] = classFactoryFunction;
    }
    std::map<std::string, std::function<std::unique_ptr<Plugin>(void)>> factoryFunctionRegistry;
};

template<class T> class Registrar {
public:
    Registrar(std::string className) {
        // register the class factory function
        PluginFactory::Instance()->RegisterFactoryFunction(className,
                [](void) -> std::unique_ptr<Plugin> { return std::make_unique<T>(); });
    }
};
