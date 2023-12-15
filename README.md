# Supermarket cashier simulation

This project is a multi-processing simulation of a supermarket environment, focusing on the behavior of cashiers while scanning customers' purchased items. Customers arrive randomly, select items with varied quantities, and spend random durations shopping. The simulation incorporates factors like cashier queue size, total items in shopping carts, scanning speed, and cashier behavior to determine customers' choices. Cashiers' positive behavior decreases over time, leading to their exit. The application employs various IPC techniques (message queues, semaphores, shared memory and signals). Additionally, it integrates OpenGL for a graphical representation of the simulation, offering users a visual insight into the dynamic interactions within the supermarket.