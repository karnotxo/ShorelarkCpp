Welcome to CShorelark's Documentation
==================================

CShorelark is a modern C++17 port of the Shorelark project, demonstrating genetic algorithms and neural networks through an interactive bird evolution simulator.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   manual/index
   architecture/index
   examples/index
   dev-guide/index
   api/index

Quick Links
----------

* :doc:`manual/getting-started`
* :doc:`examples/basic-usage`
* :doc:`architecture/overview`
* :doc:`dev-guide/contributing`

Features
--------

* Modern C++17 implementation
* Neural network simulation
* Genetic algorithm optimization
* Real-time visualization
* Cross-platform support

.. mermaid::

   graph TD
       A[Simulation Core] --> B[Neural Network]
       A --> C[Genetic Algorithm]
       B --> D[Bird Brain]
       C --> E[Evolution]
       D --> F[Behavior]
       E --> F

Components
---------

Neural Network
~~~~~~~~~~~~~

.. doxygenclass:: NeuralNetwork
   :members:
   :protected-members:
   :private-members:

Genetic Algorithm
~~~~~~~~~~~~~~~

.. doxygenclass:: GeneticAlgorithm
   :members:
   :protected-members:
   :private-members:

Indices and Tables
-----------------

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search` 