:mod:`urequests` --- Related functions of HTTP client
=====================================================

.. module:: urequests
   :synopsis: Relevant functional functions of the HTTP client, providing various HTTP request methods

Relevant functional functions of the HTTP client, providing various HTTP request methods

Response class
--------------

.. class:: Response(s)

   The Response class object contains the server's response to the HTTP request.

   Methods
   ~~~~~~~

   .. decorator:: headers

      Returns the headers.

   .. decorator:: content

      Returns the content of the response, in bytes.

   .. method:: json()

      Return response json encoded content and convert to dict type.

Functions
---------

.. function:: request(function, url, data=None, json=None, files=None, headers={}, auth=None)

Send an HTTP request to the server.

    - ``function`` - HTTP function to use
    - ``url`` - URL to send
    - ``data`` - To append to the body of the request. If a dictionary or tuple list is provided, the form will be encoded.
    - ``json`` - json is used to attach to the body of the request.
    - ``files`` - Used for file upload, the type is 2-tuple, which defines the file name, file path and content type. As follows,{‘name’, (file directory,content-type)}
    - ``headers`` - Dictionary of headers to send.
    - ``auth`` - Auth tuple to enable Basic/Digest/Custom HTTP Auth.

.. function:: head(url, **kw)

    Send HEAD request and return Response object.

        - ``url`` - Request object URL
        - ``**kw`` - The parameters of the request function.

.. function:: get(url, **kw)

    Send GET request and return Response object.

        - ``url`` - Request object URL
        - ``**kw`` - Parameters of request function.

.. function:: post(url, **kw)

    Send POST request and return Response object.

        - ``url`` - Request object URL
        - ``**kw`` - Parameters of request function.

.. function:: put(url, **kw)

    Send PUT request and return Response object.

        - ``url`` - RRequest object URL
        - ``**kw`` - Parameters of request function.
    
.. function:: patch(url, **kw)

    Send PATCH request, return Response object.

        - ``url`` - Request object URL
        - ``**kw`` - Parameters of request function.

.. function:: delete(url, **kw)

    Send a DELETE request. Return Response object。

        - ``url`` - Request object URL
        - ``**kw`` - Parameters of request function.
