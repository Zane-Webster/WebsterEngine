#ifndef WE_RENDERER_RENDERER_H_
#define WE_RENDERER_RENDERER_H_

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "obj/Object.h"

class Renderer {
public:
    Renderer();

    void Build();

    void AddItem(std::shared_ptr<WE::RenderItem> item);
    void AddItems(std::vector<std::shared_ptr<WE::RenderItem>> items);

    void RemoveItem(std::string name);
    void RemoveItems(std::vector<std::string> names);

    void Clear();

    void RenderAll();
    
private:
    std::vector<std::shared_ptr<WE::RenderItem>> unbatched_items = WE_EMPTY_VECTOR;
    std::vector<WE::RENDER_BATCH> items = WE_EMPTY_VECTOR;

    void _MakeBatches();
};

#endif // WE_RENDERER_RENDERER_H_