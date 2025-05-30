#include "fumo_engine/serialization/fumo_serializer.hpp"

#include <fstream>

#include "filesystem"
#include "fumo_engine/all_components_macro.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/screen_components.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

namespace fs = std::filesystem;

void clear_files_for_serialization() {

    fs::copy(fs::current_path(),
             "../backup_serialized_data",
             fs::copy_options::recursive | fs::copy_options::overwrite_existing);

    for (const auto& directory : fs::directory_iterator(fs::current_path())) {

        if (!directory.is_directory()) [[unlikely]] {
            PANIC(
                "added a file into serialized_data (should only have \
                directories)");
        }
        fs::remove_all(directory);
    }
}

void fix_JSON_serialization() {
    for (const auto& directory : fs::directory_iterator(fs::current_path())) {

        if (!directory.is_directory()) [[unlikely]] {
            PANIC(
                "added a file into serialized_data (should only have directories)");
        }

        for (const auto& dir_entry : fs::directory_iterator(directory)) {
            const auto file_name = dir_entry.path();

            std::ifstream in_stream(file_name);
            std::stringstream cereal_buffer;

            if (in_stream.fail()) [[unlikely]]
                PANIC("failed to open file for ifstream.\n");

            cereal_buffer << in_stream.rdbuf();
            std::string contents = cereal_buffer.str();

            contents.insert(0, "{\n");
            contents.erase(contents.end() - 2); // remove extra "," at the end
            contents.append("}");

            std::ofstream out_stream(file_name, std::ios::trunc);
            out_stream << contents;
            PRINT_NO_NAME(std::format(
                "serialized {}.",
                (file_name.parent_path().filename() / file_name.filename())
                    .string()));
        }
    }
}

void LevelSerializer::serialize_levels() {
    // NOTE:  assumes that we have changed the directory to serialized_data/

    // TODO: add data versioning to the serialized_data

    clear_files_for_serialization();

    for (const auto& entity_id : sys_entities) {
        // FIXME: add Screen to each entity instead of the screenId

        // ---------------------------------------------------------------------
        const auto& screen =
            fumo_engine->ECS->get_component<Serialized>(entity_id);
        const auto& level_id =
            fumo_engine->ECS->get_component<LevelId>(entity_id);
        // ---------------------------------------------------------------------

        fs::path level_screen_path = std::format("level{}", level_id.level_id);
        fs::create_directory(level_screen_path);

        level_screen_path /= std::format("screen{}.json", screen.file_id);

        std::ofstream out_stream(level_screen_path, std::ios::app);

        std::stringstream cereal_buffer;

        if (out_stream.fail()) [[unlikely]]
            PANIC("failed to open or create file\n");

        {
            // guarantee that cereal flushes out its buffer
            cereal::JSONOutputArchive out_archive(cereal_buffer);
            FumoSerializer::serialize_entity(entity_id, out_archive);
        }

        std::string contents = cereal_buffer.str();
        contents = contents.substr(2, contents.length() - 4);
        contents.append(",\n");

        out_stream << contents;
    }
    // edit the file so it matches the correct JSON format
    // after writing to all files, go back and append { and } to the files
    fix_JSON_serialization();
};

// structure for the json is:
// entity id
// component_mask
// all components
namespace FumoSerializer {

#define DESERIALIZE_COMPONENT(Type) \
    fumo_engine->ECS->entity_add_components( \
        entity_id, \
        FumoSerializer::deserialize_component<Type>(#Type, \
                                                    component_id, \
                                                    in_archive))

void deserialize_component_by_id(const EntityId& entity_id,
                                 const ComponentId& component_id,
                                 cereal::JSONInputArchive& in_archive) {
#define XMACRO(Type) \
    case AllComponentTypes::Type: \
        DESERIALIZE_COMPONENT(Type); \
        break;

    switch (AllComponentTypes {component_id}) { ALL_COMPONENTS_X_MACRO() }

// undefine it so we can reuse it later
#undef XMACRO
}

void serialize_entity(const EntityId& entity_id,
                      cereal::JSONOutputArchive& out_archive) {
    auto component_mask = fumo_engine->ECS->get_component_mask(entity_id);

    out_archive(CEREAL_NVP(entity_id));
    out_archive(CEREAL_NVP(component_mask));

    // fumo_engine->ECS->debug_print_entity(entity_id);

    for (ComponentId id = 0; id < MAX_COMPONENTS; ++id) {
        uint64_t iterate = 1;
        iterate <<= id;
        if (component_mask & (iterate)) {
            fumo_engine->ECS->serialize_component(entity_id, id, out_archive);
        }
    }
}

void deserialize_entity(cereal::JSONInputArchive& in_archive) {
    // NOTE: follows the save order for serialized data
    // *MUST* save entities in entity_id order else this will break

    EntityId entity_id;
    ComponentMask component_mask;
    in_archive(entity_id, component_mask);
    entity_id = fumo_engine->ECS->create_entity();

    for (ComponentId id = 0; id < MAX_COMPONENTS; ++id) {
        uint64_t iterate = 1;
        iterate <<= id;
        if (component_mask & (iterate)) {
            FumoSerializer::deserialize_component_by_id(entity_id,
                                                        id,
                                                        in_archive);
        }
    }
}

void deserialize_levels() {
    // assumes we are in the serialized_data/ directory

    // entities are deserialized with new entity_ids that are
    // in order of screen_id and level_id

    for (const auto& directory : fs::directory_iterator(fs::current_path())) {

        if (!directory.is_directory()) [[unlikely]] {
            PANIC(
                "added a file into serialized_data (should only have directories)");
        }

        for (const auto& dir_entry : fs::directory_iterator(directory)) {
            const auto file_name = dir_entry.path();

            std::ifstream in_stream(file_name);

            if (in_stream.fail()) [[unlikely]]
                PANIC("failed to open file for ifstream.\n");

            cereal::JSONInputArchive in_archive(in_stream);

            try {
                while (1) {
                    FumoSerializer::deserialize_entity(in_archive);
                }
            } catch (cereal::Exception) {
                // finished going through file
                PRINT_NO_NAME(std::format(
                    "FUMO_SERIALIZER: parsed {}.",
                    (file_name.parent_path().filename() / file_name.filename())
                        .string()));
                break;
            }
        }
    }
}

} // namespace FumoSerializer
