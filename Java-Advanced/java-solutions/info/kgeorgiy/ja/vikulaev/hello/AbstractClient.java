package info.kgeorgiy.ja.vikulaev.hello;

import info.kgeorgiy.java.advanced.hello.HelloClient;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.UnknownHostException;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public abstract class AbstractClient implements HelloClient {

    protected static final Charset CHARSET = StandardCharsets.UTF_8;
    protected static final int SOCKET_TIMEOUT = 200;
    protected static String HOST;
    protected static String PREFIX;
    protected static int THREADS;
    protected static int PORT;
    protected static int REQUESTS;

    protected static final String READ_ERROR = "Unable to read response.";
    protected static final String WRITE_ERROR = "Unable to write request";

    protected static void main(final AbstractClient abstractClient, final String[] args) {
        try {
            checkArgs(args);
        } catch (IllegalArgumentException e) {
            System.err.println(e.getMessage());
            return;
        }
        HOST = args[0];
        PORT = Integer.parseInt(args[1]);
        PREFIX = args[2];
        THREADS = Integer.parseInt(args[3]);
        REQUESTS = Integer.parseInt(args[4]);
        try {
            abstractClient.run(HOST, PORT, PREFIX, THREADS, REQUESTS);
        } catch (RuntimeException e) {
            System.err.println(e.getMessage());
        }
    }

    @Override
    public void run(String host, int port, String prefix, int threads, int requests) {
        HOST = host;
        PORT = port;
        PREFIX = prefix;
        THREADS = threads;
        REQUESTS = requests;
        ExecutorService executorService = Executors.newFixedThreadPool(THREADS);
        SocketAddress address;
        try {
            address = new InetSocketAddress(InetAddress.getByName(HOST), PORT);
        } catch (UnknownHostException e) {
            throw new RuntimeException("Unknown host. " + e.getMessage());
        }
        try {
            runClient(address, executorService);
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
    }

    protected abstract void runClient(SocketAddress address, ExecutorService executorService) throws IOException;

    private static void checkArgs(String[] args) {
        String ERROR = """
                You must give 5 parameters to program.
                \t1)Name or IP-address [String value]
                \t2)Port number [Numeric value]
                \t3)Prefix [String value]
                \t4)Number of threads [Numeric value]
                \t5)Number of posts per thread [Numeric value]""";
        if (args == null || args.length != 5) {
            throw new IllegalArgumentException(ERROR);
        }
        for (String arg : args) {
            if (arg == null) {
                throw new IllegalArgumentException(ERROR);
            }
        }
        try {
            Integer.parseInt(args[1]);
            Integer.parseInt(args[3]);
            Integer.parseInt(args[4]);
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Port, number of threads and requests must be numeric values");
        }
    }

    public static String requestName(int thread, int request) {
        return HelloUtils.requestName(PREFIX, thread, request);
    }
}
