# **Spectrum**

**Spectrum** is an extensible rendering engine built to unify real-time rasterization and offline ray tracing within a single, coherent framework. It serves both as a practical renderer and an educational platform—simple at its foundation, yet designed to evolve into a far more capable engine in the future.

Spectrum treats rasterization and ray tracing not as competing paradigms, but as complementary techniques. Use whichever best fits your task—or combine them when appropriate.

---

## **Rendering Capabilities**

### **Real-Time Rasterization**
- Software rasterizer for clarity and experimentation  
- GPU-accelerated rasterization using:  
  - **Vulkan** (Windows & Linux)  
  - **Metal** (macOS)

### **Offline Ray Tracing**
- CPU-based reference implementation  
- GPU-accelerated ray tracing via **Vulkan RT extensions**, with optional **CUDA/OptiX**  
- Progressive refinement workflow: from fast preview to final-quality results

---

## **Focus**

Spectrum is designed for users who value understanding, control, and clarity:

- **Researchers** seeking a clean, readable foundation for prototyping new rendering techniques  
- **Developers** building games, tools, or visualizations who want power without unnecessary complexity  

Spectrum prioritizes **depth over breadth**. Growth is deliberate—we do not chase features for their own sake.

---

## **Dependencies**

- Minimal platform libraries  
- Optional: **Vulkan SDK**

---

## **License**

**TBD**
