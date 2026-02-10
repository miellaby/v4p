# Optimized Integer Scaling for 16-bit MCUs

On 16-bit architectures, the standard formula for scaling a vertex —`(x * target) / original`— is dangerous. 

**Example:** Scale $x = 500$ from a $640$ grid to a $700$ grid.
* **Math:** $500 \times 700 = 350,000$.
* **The Crash:** $350,000$ exceeds the limit of a 16-bit signed integer (`32,767`) and even an unsigned 16-bit integer (`65,535`).

To avoid overflow and the overhead of floating-point computation, we use **Quotient-Remainder Scaling**. This technique breaks the scaling factor into a **Whole Number** and a **Remainder**, keeping intermediate products within safe limits.

### The technique
1.  **Pre-calculate** (once per transformation):
    * `whole = targetSize / originalSize`
    * `rem = targetSize % originalSize`
2.  **Apply to Vertex** ($x$):
    * $x_{new} = (x \times whole) + \frac{(x \times rem) + (originalSize / 2)}{originalSize}$

The formula works with shrinking (whole == 0). Adding `orig / 2` to the numerator, ensuring the vertex rounds to the **nearest** pixel.
