package info.kgeorgiy.ja.vikulaev.hello;

import info.kgeorgiy.java.advanced.hello.HelloServer;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;
import java.util.concurrent.*;

public abstract class AbstractServer implements HelloServer {

    protected static final Charset CHARSET = StandardCharsets.UTF_8;
    protected static final int POOL_SIZE = 1 << 17;
    protected static final int AWAIT_TIMEOUT = 5;

    protected static int PORT;
    protected static int THREADS;

    ExecutorService receiver = Executors.newSingleThreadExecutor();

    ExecutorService workers;

    private static void checkArgs(String[] args) {
        String ERROR = """
                To run server you need to give:
                \t1) Port number [Numeric value]
                \t2) Number of threads [Numeric value]""";
        if (args == null || args.length != 2) {
            throw new IllegalArgumentException(ERROR);
        }
        for (var arg : args) {
            if (arg == null) {
                throw new IllegalArgumentException(ERROR);
            }
            try {
                Integer.parseInt(arg);
            } catch (NumberFormatException e) {
                throw new IllegalArgumentException("Both port number and number of threads must be numeric value");
            }
        }
    }

    protected static void main(AbstractServer server, String[] args) {
        try {
            checkArgs(args);
        } catch (IllegalArgumentException e) {
            System.err.println(e.getMessage());
            return;
        }
        try {
            server.start(Integer.parseInt(args[0]), Integer.parseInt(args[1]));
        } catch (IllegalArgumentException e) {
            System.err.println(e.getMessage());
            return;
        }
        Scanner scanner = new Scanner(System.in);
        while (true) {
            if (scanner.nextLine().equals("close")) {
                server.close();
                return;
            }
        }
    }

    @Override
    public void start(int port, int threads) {
        PORT = port;
        THREADS = threads;
        workers = new ThreadPoolExecutor(THREADS, THREADS, 2,TimeUnit.MINUTES,
                new ArrayBlockingQueue<>(POOL_SIZE), new ThreadPoolExecutor.DiscardPolicy());
        try {
            serverStart();
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
    }

    protected abstract void serverStart() throws IOException;

    @Override
    public void close() {
        receiver.shutdownNow();
        workers.shutdownNow();

        try {
            if (!workers.awaitTermination(AWAIT_TIMEOUT, TimeUnit.SECONDS)) {
                System.err.println("Some tasks were not terminated.");
            }
        } catch (InterruptedException ignored) {
            Thread.currentThread().interrupt();
        }
    }


}
