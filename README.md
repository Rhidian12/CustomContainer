# CustomContainer

### The Goal
I started this small project with the hope of being able to make my own version of `std::vector` and possibly even improve it.

### The Code
One of the things that makes `std::vector` as fast as it is, is its contiguous memory. For those unfamiliar with the term: contiguous memory means that all memory is guaranteed to be next to eachother in memory, rather than be scattered around (as might happen with a linked list).
</br> So, I set out to make my own container with contiguous memory. There are 2 ways I could have done this:
1. Make a wrapper around a C-style array, since those are guaranteed to be contiguous.
2. Use `calloc` or `malloc` to get a raw block of memory and work with that.

I decided to go with option 2, since option 1 is something we did in class and I wanted to work with pointers.
After deciding this, I created 3 pointers: `m_pHead` would be the start of my block of memory, `m_pTail` would be the end of the block and `m_pCurrentElement` would be the current element (imaginative naming).
</br> The constructor speaks for itself, it assigns a block of memory with a size of `5 * sizeof(Type)` and makes sure that the `m_pTail` is also initialized.
Next up, the `Push_back`. This was not too hard either, all I needed to do was:
* Check if this is the first time an element gets added to the container
  * If no elements have been added yet: Just add the element to the container by moving `m_pCurrentElement`
  * If elements have been added already: First check if memory needs to be reallocated, then add it to the container by moving `m_pCurrentElement`

This is easy enough, however, reallocating memory actually took a lot longer than I'd care to admit.
</br> Most of my problems reallocating memory came from an issue I completely forgot: `int* pTest{ (int*)100 }; pTest += 5;` is **not** the same as doing `int* pTest{ (int*)100 }; pTest += 5 * sizeof(int);`
</br> Adding 5 to an int pointer is adding 5 times 4 bytes, thus 20 bytes in total. Adding `5 * sizeof(int)` to an int pointer is adding 20 times 4 bytes, thus adding **80** bytes in total. I made this mistake for a *very* long time, making it that my Container always crashed on destruction, until someone was kind enough to point it out to me.
</br> After that issue was fixed, the rest of the `Reallocate()` function was far more trivial, merely a matter of moving values and pointers around, and making sure that the old memory gets released.

I now had the 2 most important aspects of the Container finished: I could add dynamically add elements into a contiguous memory which resized itself when needed.
</br> I added some functionality to my Container, such as `GetSize()`, `GetCapacity()` and `operator []`, until I stumbled into a bit of a problem with the `Clear()` function.
However, once again, this function was merely a byproduct of my earlier mismanagement of pointers, and was thus quickly fixed.
</br> And with that, I had myself a fully functional `CustomContainer`

### Performance
I am sad to say that `std::vector` is utterly superior to my CustomContainer.

![STLvsCustom](https://github.com/Rhidian12/CustomContainer/blob/main/STLvsCUSTOM.png)

As this graph clearly shows, STL is a **lot** faster. This test was run in Debug mode, but the times have the same amount of difference between them in Release mode.

I might later update this code, when I understand how to use and apply SIMD operations, which will probably make my container a lot faster
