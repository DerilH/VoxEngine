// #include "../../../Engine/include/VoxEngine/ShaderCompiler.h"
// #include "../src/core/Logger.h"
//
// int main() {
//     const Vox::Render::Shader::ShaderCompiler compiler;
//     LOG_INFO("Compiling shaders");
//     const auto mCompiled = compiler.compileShaders(std::filesystem::path("../shaders"));
//
//     for (const auto &shader: mCompiled) {
//         try {
//             const auto path = std::filesystem::path("../shadersCache/") / (shader.name + ".spv");
//             shader.save(path);
//             LOG_INFO(shader.name + "  - mCompiled");
//         } catch (const std::exception &e) {
//             LOG_WARN(shader.name + "  - failed");
//         }
//     }
//     LOG_INFO("Compilation ended");
//     return 0;
// }
