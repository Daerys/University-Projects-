package info.kgeorgiy.ja.vikulaev.hello;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.util.Iterator;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.ArrayBlockingQueue;

import static info.kgeorgiy.ja.vikulaev.hello.HelloUtils.printError;

public class HelloUDPNonblockingServer extends AbstractServer {
    final static String RESPONSE_PREFIX = "Hello, ";

    private DatagramChannel datagramChannel;
    private Selector selector;
    private final Queue<Process> queue = new ArrayBlockingQueue<>(POOL_SIZE);

    private static class Process {
        SocketAddress address;
        ByteBuffer buffer;

        Process(SocketAddress address, ByteBuffer buffer) {
            this.address = address;
            this.buffer = buffer;
        }

    }

    public static void main(String[] args) {
        AbstractServer.main(new HelloUDPNonblockingServer(), args);
    }

    @Override
    protected void serverStart() throws IOException {
        datagramChannel = DatagramChannel.open();
        datagramChannel.configureBlocking(false);
        datagramChannel.bind(new InetSocketAddress(PORT));
        selector = Selector.open();

        datagramChannel.register(selector, SelectionKey.OP_READ);

        receiver.submit(() -> {
            while (!Thread.interrupted()) {
                try {
                    selector.select(100); // :NOTE: константа
                    processSelectedKeys();
                } catch (IOException e) {
                    printError("Unable to selects a set of keys.", e.getMessage());
                }
            }
        });
    }

    private void processSelectedKeys() {
        Set<SelectionKey> selectedKeys = selector.selectedKeys();
        Iterator<SelectionKey> keyIterator = selectedKeys.iterator();
        while (keyIterator.hasNext()) {
            SelectionKey key = keyIterator.next();
            keyIterator.remove();
            if (key.isReadable()) {
                read(key);
                continue;
            }
            write(key);
        }
    }

    private void write(SelectionKey key) {
        Process op = queue.poll();
        if (op == null) {
            key.interestOps(SelectionKey.OP_READ);
            return;
        }
        SocketAddress address = op.address;
        ByteBuffer buffer = op.buffer;
        try {
            datagramChannel.send(buffer, address);
        } catch (IOException e) {
            printError("Unable to send a response.", e.getMessage());
        }
    }

    private void read(SelectionKey key) {
        DatagramChannel channel = (DatagramChannel) key.channel();
        ByteBuffer buffer;
        try {
            buffer = ByteBuffer.allocate(channel.socket().getReceiveBufferSize());
        } catch (SocketException e) {
            printError("Unable to create buffer for request.", e.getMessage());
            return;
        }
        SocketAddress address;
        try {
            address = channel.receive(buffer);
        } catch (IOException e) {
            printError("Unable to get client's address.", e.getMessage());
            return;
        }

        if (address != null) {
            buffer.flip();
            ByteBuffer newBuffer = ByteBuffer.allocate(RESPONSE_PREFIX.length() + buffer.remaining());
            newBuffer.put(RESPONSE_PREFIX.getBytes());
            newBuffer.put(buffer);
            newBuffer.flip();
            queue.add(new Process(address, newBuffer));
            key.interestOps(SelectionKey.OP_WRITE);
        }
    }

    @Override
    public void close() {
        super.close();
        try {
            datagramChannel.close();
            selector.close();
        } catch (IOException ignore) {
        }
    }
}
