/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Buffer.hpp"

#include "GL.hpp"

namespace OpenGL
{
    BufferHandle CreateBuffer(BufferType type, GLsizeiptr size_in_bytes) noexcept
    {
        // TODO Generate a new OpenGL buffer, bind it, allocate `size_in_bytes` bytes of memory for it (with no initial data), then unbind the buffer and return
        // https://docs.gl/es3/glGenBuffers
        // https://docs.gl/es3/glBindBuffer
        // https://docs.gl/es3/glBufferData
        return 0;
    }

    BufferHandle CreateBuffer(BufferType type, std::span<const std::byte> static_buffer_data) noexcept
    {
        // TODO Generate a new OpenGL buffer, bind it, upload the data in `static_buffer_data` to it, then unbind the buffer and return
        // https://docs.gl/es3/glGenBuffers
        // https://docs.gl/es3/glBindBuffer
        // https://docs.gl/es3/glBufferData
        return 0;
    }

    void UpdateBufferData(BufferType type, BufferHandle buffer, std::span<const std::byte> data_to_copy, GLsizei starting_offset) noexcept
    {
        // TODO Bind the buffer, update its data starting at `starting_offset` with the data in `data_to_copy`, then unbind the buffer
        // https://docs.gl/es3/glBindBuffer
        // https://docs.gl/es3/glBufferSubData
    }
}
