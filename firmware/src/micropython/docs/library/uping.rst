:mod:`uping` --- Ping another computer
======================================

.. module:: uping
   :synopsis: Ping another computer

.. function:: ping(host, count=4, timeout=5000, interval=10, quiet=False, size=64)

    Ping the ``host`` with ``count`` packets each having ``timeout`` at a
    rate between packets of ``interval``. If ``quiet`` is ``True`` do not print
    stats on return. Packet sizes are ``size`` bytes each.

    Returns a tuple containing the number of transmitted packets and the number received.
