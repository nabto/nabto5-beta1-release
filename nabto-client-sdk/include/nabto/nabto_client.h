#ifndef NABTO_CLIENT_API_H
#define NABTO_CLIENT_API_H

/**
 * Nabto Client C API.
 *
 * Nabto provides a platform for connecting applications with
 * devices. The platform consists of three major parts.
 *
 * Vocabulary:
 * Client: Clients are often apps where this library is embedded
 * inside. The clients can make connections to devices. Using the
 * servers.
 *
 * Device: Devices is often embedded devices running the Nabto
 * Embedded SDK, e.g. a heating control system or an ip camera.
 *
 * Server: Servers are hosted in datacenters and makes it possible to
 * create connections between the clients and devices.
 */

#if defined(_WIN32)
#define NABTO_CLIENT_API __stdcall
#if defined(NABTO_CLIENT_WIN32_API_STATIC)
#define NABTO_CLIENT_DECL_PREFIX extern
#elif defined(NABTO_CLIENT_CLIENT_API_EXPORTS)
#define NABTO_CLIENT_DECL_PREFIX __declspec(dllexport)
#else
#define NABTO_CLIENT_DECL_PREFIX __declspec(dllimport)
#endif
#else
#define NABTO_CLIENT_API
#define NABTO_CLIENT_DECL_PREFIX extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

typedef uint64_t nabto_client_uint64_t;
typedef nabto_client_uint64_t nabto_client_duration_t;

typedef enum NabtoClientLogLevel_ {
    NABTO_CLIENT_LOG_DISABLED,
    NABTO_CLIENT_LOG_CRITICAL,
    NABTO_CLIENT_LOG_ERROR,
    NABTO_CLIENT_LOG_WARN,
    NABTO_CLIENT_LOG_INFO,
    NABTO_CLIENT_LOG_DEBUG,
    NABTO_CLIENT_LOG_TRACE
} NabtoClientLogLevel;

typedef enum NabtoClientErrorSource_ {
    NCES_API, // Originates from the use of the api.
    NCES_RELAY, // Originates from the communication with the relay server.
    NCES_STREAM, // Originates from streaming layer.
    NCES_CONNECTION, // Originates from the connection between this
                     // client and a device.
    NCES_COAP // originates from coap
} NabtoClientErrorSource;

#define NABTO_CLIENT_ERROR_MAPPING(XX) \
    XX(NCES_API, NABTO_CLIENT_API_FUTURE_NOT_READY, 1, "The future is not ready") \
    XX(NCES_API, NABTO_CLIENT_API_UNKNOWN_ERROR, 2, "Unknown error")    \
    XX(NCES_API, NABTO_CLIENT_API_MISSING_DEVICE_ID, 3, "Missing remote peer id") \
    XX(NCES_API, NABTO_CLIENT_API_OPERATION_IN_PROGRESS, 4, "Operation in progress") \
    XX(NCES_API, NABTO_CLIENT_API_OPERATION_ABORTED, 5, "Operation aborted") \
    XX(NCES_RELAY, NABTO_CLIENT_RELAY_HOSTNAME_LOOKUP_FAILED, 1, "Could not resolve dns name for relay server") \
    XX(NCES_RELAY, NABTO_CLIENT_RELAY_COMMUNICATION_ERROR, 2, "Could not communicate with the relay server") \
    XX(NCES_CONNECTION, NABTO_CLIENT_CONNECTION_CLOSED,    1, "The Connection is closed") \
    XX(NCES_STREAM, NABTO_CLIENT_STREAM_CLOSED, 2, "The stream is closed") \
    XX(NCES_STREAM, NABTO_CLIENT_STREAM_EOF, 3, "There is no more data to be read from the stream") \
    XX(NCES_STREAM, NABTO_CLIENT_STREAM_ABORTED, 4, "The stream has been aborted.") \
    XX(NCES_COAP, NABTO_CLIENT_COAP_NO_CONTENT_TYPE, 1, "There is no content type") \
    XX(NCES_COAP, NABTO_CLIENT_COAP_NO_RESPONSE, 2, "The response is not ready") \
    XX(NCES_COAP, NABTO_CLIENT_COAP_DECODE_ERROR, 3, "Coap decode error") \
    XX(NCES_COAP, NABTO_CLIENT_COAP_TIMEOUT, 4, "Coap timeout")         \


#define NABTO_CLIENT_ERROR(source, code) ((source << 24) | code)

typedef enum NabtoClientError_ {
  NABTO_CLIENT_OK = 0,
#define XX_ERROR(source, name, code, _) name = NABTO_CLIENT_ERROR(source, code),
  NABTO_CLIENT_ERROR_MAPPING(XX_ERROR)
#undef XX_ERROR
  /* @cond IGNORE */
  NABTO_CLIENT_ERROR_LAST_ENTRY
  /* @endcond*/
} NabtoClientError;


typedef enum NabtoClientConnectionType_ {
    NABTO_CLIENT_CONNECTION_TYPE_RELAY, // the connection is a relay connection
    NABTO_CLIENT_CONNECTION_TYPE_DIRECT // the connection is a direct udp connection either p2p or local.
} NabtoClientConnectionType;


/**
 * A NabtoClientContext is a context holding common state across
 * connections.
 */
typedef struct NabtoClientContext_ NabtoClientContext;

/**
 * A nabto connection is the holder of a peer to peer connection between
 * this client and a remote device.
 */
typedef struct NabtoClientConnection_ NabtoClientConnection;

/**
 * A nabto stream is a bidirectional stream of bytes on top of a nabto connection
 */
typedef struct NabtoClientStream_ NabtoClientStream;

/**
 * A Nabto future is used for all async funtions, we deliver some
 * functions such that they can be handled blocking, but for an event
 * driven architecture, the future must be handled using the
 * callbacks. It's up to the application to make the machinery that
 * handles the callbacks.
 */
typedef struct NabtoClientFuture_ NabtoClientFuture;

/**
 * Callback from a future when it is resolved.
 */
typedef void (*NabtoClientFutureCallback)(NabtoClientFuture* future, void* data);


typedef struct NabtoClientLogMessage_ {
    nabto_client_uint64_t timestamp; // ms since epoch
    NabtoClientLogLevel severity;
    const char* file;
    int line;
    const char* message; /** the message null terminated utf-8 */
} NabtoClientLogMessage;

typedef void (*NabtoClientLogCallback)(const NabtoClientLogMessage* message, void* data);

typedef struct NabtoKeepAliveSettings_ {
    int interval; // interval between probes in seconds.
    int retryInterval; // interval between retries in seconds.
    int maxRetries; // max number of retries.
} NabtoKeepAliveSettings;

/***************
 * Context API *
 ***************/

/**
 * The Context is a common base for connections. Some resources
 */

/**
 * Create a context
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientContext* NABTO_CLIENT_API
nabto_client_context_new();

/**
 * Free a context
 */
NABTO_CLIENT_DECL_PREFIX void NABTO_CLIENT_API
nabto_client_context_free(NabtoClientContext* context);


/******************
 * Connection API *
 ******************/

/**
 * A connection is used to make a connection to a specific device.  The
 * connection contains options to specify how the connect should
 * happen.
 */

/**
 * Create a new nabto connection
 *
 * @return A new connection or NULL.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientConnection* NABTO_CLIENT_API
nabto_client_connection_new(NabtoClientContext* context);

/**
 * Free a connection
 *
 * @param connection the connection to be freed.
 */
NABTO_CLIENT_DECL_PREFIX void NABTO_CLIENT_API
nabto_client_connection_free(NabtoClientConnection* connection);

/**
 * Set the product id and device id for the remote device.
 *
 * This function is required to be called before connecting to a
 * device. It cannot be changed after a connection is made.
 *
 * @param connection  the connection.
 * @param productId the product id aka the id for the specific group of devices.
 * @param deviceId the unique id for the device.
 * @return NABTO_CLIENT_OK if the id was set.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_product_id(NabtoClientConnection* connection, const char* productId);

NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_device_id(NabtoClientConnection* connection, const char* deviceId);


/**
 * Set the server api key, which is provided by nabto. Each APP needs
 * its own server api key to be able to connect to the nabto api. The
 * server api key is used to distinguish different apps. Since the
 * server api key will be put into the final applications it's not
 * secret.
 *
 * @param connection the connection
 * @param serverApiKey the clientId
 * @return NABTO_CLIENT_OK on success
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_server_api_key(NabtoClientConnection* connection,
                                    const char* serverApiKey);

/**
 * Set a JWT token to use when connecting to the relay server.
 *
 * If the user is authenticated and can get a JWT this JWT can be
 * given to the connect such that the relay server can validate that
 * the given user has access to connect to the specific device.
 *
 * The server will look for the following claims:
 *   nabto_client_product_id
 *   nabto_client_device_id
 *
 * @param connection the connection
 * @param jwt the base64 JWT string.
 * @return NABTO_CLIENT_OK on success
 */
// TODO
/* NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API */
/* nabto_client_connection_set_relay_jwt(NabtoClientConnection* connection, */
/*                                const char* jwt); */

/**
 * Provide information about the application which uses nabto, such
 * that it's easier to understand what apps has what communicaton
 * behavior. The application name is also present in central
 * connection information.
 *
 * @param connection the connection
 * @param applicationName if non null the application name
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_application_name(NabtoClientConnection* connection,
                                             const char* applicationName);

/**
 * provide a version number for the application running nabto. This
 * information is used to see if a specific application version is
 * having a different behavior than other versions of the same app.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_application_version(NabtoClientConnection* connection,
                                                const char* applicationVersion);

/**
 * Override the default relay dispatcher endpoint. This is the initial
 * server the client connects to find and make a remote connection to
 * the remote peer. The default endpoint is
 * https://<productid>.clients.nabto.com. This is only needed if the
 * solution is deployed as a standalone solution with selfmanaged dns.
 *
 * This needs to be set before the connect is initiated to take
 * effect.
 *
 * @param connection the connection
 * @param endpoint the endpoint to use.
 * @return NABTO_CLIENT_OK if set.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_server_url(NabtoClientConnection* connection,
                                       const char* endpoint);
/**
 * sets a private key pair for a connection.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_private_key(NabtoClientConnection* connection, const char* privateKey);


/**
 * Set keep alive settings.
 *
 * Keep alive settings is used for keeping connections alive and for
 * detecting dead connections. It works by sending keep alives if no
 * other traffic has been on the line. It can be used to get faster
 * response on dead connections if that's important for the user
 * excperience.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_set_keep_alive_settings(NabtoClientConnection* connection, NabtoKeepAliveSettings settings);


/**
 * Get the fingerprint of the remote device public key. The
 * fingerprint is used to validate the identity of the remote device.
 *
 * @param connection The connection.
 * @param fingerprint A pointer to an array large enough for the fingerprint.
 * @return NABTO_CLIENT_OK if the fingerprint was copied to the fingerprint parameter.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_get_device_fingerprint(NabtoClientConnection* connection, uint8_t* fingerprint);

/**
 * Get the connection type. Use this function to limit the amount of
 * traffic sent over relay connections.
 *
 * @param connection The connection.
 * @param type the connection type.
 * @return NABTO_CLIENT_OK if the connection is connected.
 *         NABTO_CLIENT_CONNECTION_CLOSED if the connection is closed or not opened yet.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_connection_get_type(NabtoClientConnection* connection, NabtoClientConnectionType* type);

/**
 * Connect to a device.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientFuture* NABTO_CLIENT_API
nabto_client_connection_connect(NabtoClientConnection* connection);

/**
 * Graceful close a connection, and the connection.  non graceful
 * closedown can be made using the free function.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientFuture* NABTO_CLIENT_API
nabto_client_connection_close(NabtoClientConnection* connection);

/*****************
 * Streaming API *
 *****************/

/**
 * The streaming api is used to make a reliable stream on top of a
 * connection. The stream is reliable and ensures data is received
 * ordered and complete. If either of these conditions cannot be met,
 * the stream will be closed in such a way that it's detectable.
 */

/**
 * Create a stream
 *
 * @param connection  The connection to make the stream on, the connection needs
 * to be kept alive until the stream has been freed.
 * @return  NULL if the stream could not be created, non NULL otherwise.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientStream* NABTO_CLIENT_API
nabto_client_stream_new(NabtoClientConnection* connection);

/**
 * Free a stream
 *
 * @param stream, the stream to free
 */
NABTO_CLIENT_DECL_PREFIX void NABTO_CLIENT_API
nabto_client_stream_free(NabtoClientStream* stream);

/**
 * Handshake a stream. This function initializes and does a three way
 * handshake on a stream.
 *
 * @param stream, the stream to connect.
 * @param contentType  The contentType to use for the stream.
                       This is used to distinguish streams in the other end.
 * @return a future when resolved the stream is either established or failed.
 *
 * Future status:
 *   NABTO_CLIENT_OK if opening went ok.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientFuture* NABTO_CLIENT_API
nabto_client_stream_open(NabtoClientStream* stream, uint32_t contentType);

/**
 * Read exactly n bytes from a stream
 *
 * if (readLength != bufferLength) the stream has reached a state
 * where no more bytes can be read.
 *
 * @param stream, the stream to read bytes from.
 * @param buffer, the buffer to put data into.
 * @param bufferLength, the length of the output buffer.
 * @param readLength, the actual number of bytes read.
 * @return  a future which resolves with ok or an error.
 *
 * Future status:
 *  NABTO_CLIENT_OK   if all data was read.
 *  NABTO_CLIENT_STREAM_EOF  if only some data was read and the stream is eof.
 *  NABTO_CLIENT_STREAM_ABORTED if the stream is aborted.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientFuture* NABTO_CLIENT_API
nabto_client_stream_read_all(NabtoClientStream* stream, void* buffer, size_t bufferLength, size_t* readLength);

/**
 * Read some bytes from a stream.
 *
 * Read atleast 1 byte from the stream, unless an error occurs or the
 * stream is eof.
 *
 * @param stream        The stream to read bytes from
 * @param buffer        The buffer where bytes is copied to,
 * @param bufferLenght  The length of the output buffer
 * @param readLength    The actual number of read bytes.
 * @return  a future which resolves to ok or a stream error.
 *
 * Future status:
 *  NABTO_CLIENT_OK if some bytes was read.
 *  NABTO_CLIENT_STREAM_EOF if stream is eof.
 *  NABTO_CLIENT_STREAM_ABORTED if the stream is aborted.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientFuture* NABTO_CLIENT_API
nabto_client_stream_read_some(NabtoClientStream* stream, void* buffer, size_t bufferLength, size_t* readLength);

/**
 * Write bytes to a stream.
 *
 * When the future resolves the data is only written to the stream,
 * but not neccessary acked. This is why it does not make sense to
 * return a number of actual bytes written in case of error since it
 * says nothing about the number of acked bytes. To ensure that
 * written bytes have been acked, a succesful call to
 * nabto_client_stream_close is neccessary after last call to
 * nabto_client_stream_write.
 *
 * @param stream, the stream to write data to.
 * @param buffer, the input buffer with data to write to the stream.
 * @param bufferLenth, length of the input data.
 * @return a future when resolved the data is written to the stream.
 *
 * Future status:
 *  NABTO_CLIENT_OK if write was ok.
 *  NABTO_CLIENT_STREAM_CLOSED if the stream is closed for writing.
 *  NABTO_CLIENT_STREAM_ABORTED if the stream is aborted.
 *
 * TODO clarify what happens when a stream is closed while a call to write is in progress.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientFuture* NABTO_CLIENT_API
nabto_client_stream_write(NabtoClientStream* stream, const void* buffer, size_t bufferLength);

/**
 * Close a stream. When a stream has been closed no further data can
 * be written to the stream. Data can however still be read from the
 * stream until the other peer closes the stream.
 *
 * When close returns all written data has been acknowledged by the
 * other peer.
 *
 * @param stream, the stream to close.
 *
 * Future status:
 *  NABTO_CLIENT_STREAM_OK if the stream is closed for writing.
 *  NABTO_CLIENT_STREAM_ABORTED if the stream is aborted.
 */

NABTO_CLIENT_DECL_PREFIX NabtoClientFuture* NABTO_CLIENT_API
nabto_client_stream_close(NabtoClientStream* stream);

/**************
 * Future API *
 **************/

/**
 * We have made a future api such that it's easier to get all the
 * different async models from a simple standard api.
 *
 * We could have implemented all the future functions for each async
 * function but that would lead to a lot of specialized functions
 * doing almost the same thing.
 */

/**
 * Free a future.
 */
NABTO_CLIENT_DECL_PREFIX void NABTO_CLIENT_API
nabto_client_future_free(NabtoClientFuture* future);

/**
 * Query if a future is ready.
 *
 * @param future, the future.
 * @return NABTO_CLIENT_OK if the future is ready else NABTO_CLIENT_API_FUTURE_NOT_READY
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_future_ready(NabtoClientFuture* future);

/**
 * Set a callback to be called when the future resolves
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_future_set_callback(NabtoClientFuture* future,
                          NabtoClientFutureCallback callback,
                          void* data);
/**
 * Wait until a future is resolved.
 */
NABTO_CLIENT_DECL_PREFIX void NABTO_CLIENT_API
nabto_client_future_wait(NabtoClientFuture* future);

/**
 * Wait atmost duration milliseconds for the future to be resolved.
 */
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_future_timed_wait(NabtoClientFuture* future, nabto_client_duration_t duration);

NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_future_error_code(NabtoClientFuture* future);


/*************
 * Error API *
 *************/
NABTO_CLIENT_DECL_PREFIX const char* NABTO_CLIENT_API
nabto_client_error_get_message(NabtoClientError error);

/********
 * Misc *
 ********/

/**
 * Return the version of the nabto client library.
 */
NABTO_CLIENT_DECL_PREFIX const char* NABTO_CLIENT_API
nabto_client_version();


/***********
 * Logging *
 ***********/
NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_set_log_callback(NabtoClientLogCallback, void* data);

NABTO_CLIENT_DECL_PREFIX NabtoClientError NABTO_CLIENT_API
nabto_client_set_log_level(NabtoClientLogLevel level);

#ifdef __cplusplus
} // extern c
#endif

#endif // NABTO_CLIENT_CLIENT_API_H
