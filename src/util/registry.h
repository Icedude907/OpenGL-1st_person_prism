#pragma once

#include <type_traits>
#include <optional>
#include <set>
#include <map>
#include <unordered_map>

namespace Util{

    // template<typename ID = size_t>
    // class IDStore{
    //     public:
    //     std::set<ID> store;
        
    //     // Returns the first ID free in the list.
    //     ID findFree(){
    //         ID prevVal; // = 0
    //         if(store.empty()) return prevVal;
    //         for (auto it = store.begin(); it != store.end(); it++){
    //             if(prevVal < it.val){ // Aka there are unused ID's between the two
    //                 return prevVal;
    //             }
    //             prevVal++;
    //         }
    //         return prevVal;
    //     }
    //     // Gets a new ID for use
    //     inline ID next(){
    //         ID id = findFree();
    //         auto [element, inserted] = store.insert(id);
    //         return inserted ? std::optional<ID>(id) : std::nullopt;
    //     }
    //     // Returns true if erased something.
    //     inline bool erase(ID id){ return store.erase(id); }
    //     inline size_t size(){ return store.size(); }
    // };

    /** Registry: Holds information associated with an ID that never changes after registering.
     *  If an ID is unregistered, that ID is reregistered upon the next allocation.
     */
    template<typename Value, template <class, class, class ...> class Container, typename ID = size_t, typename ...ContainerExtras>
    class Registry{
        using Storage = Container<ID, Value, ContainerExtras...>;
        static_assert(std::is_same_v<Storage, std::map<ID, Value, ContainerExtras...>> || std::is_same_v<Storage, std::unordered_map<ID, Value, ContainerExtras...>>, "Underlying storage type must be a std::map-ish.");
        static_assert(std::is_integral_v<ID>, "Registry ID must be an integer value"); // TODO: make restriction based on operator++ and operator<
        
        public:
        std::map<ID, Value> store;
        
        // Returns the first ID free in the list.
        ID findFree(){
            static ID prevVal; // = 0 (set once)
            if(store.empty()) return prevVal;
            for (auto it = store.begin(); it != store.end(); it++){
                ID currentVal = it->first;
                if(prevVal < currentVal){ // Aka there are unused ID's between the two
                    return prevVal;
                }
                prevVal++;
            }
            return prevVal;
        }
        std::optional<ID> insert(const Value& value){
            ID id = findFree();
            auto [element, inserted] = store.insert({id, value});
            return inserted ? std::optional<ID>(id) : std::nullopt;
        }
        // Returns true if erased something.
        inline bool erase(ID id){ return store.erase(id); }
        inline size_t size(){ return store.size(); }
        
    };
}