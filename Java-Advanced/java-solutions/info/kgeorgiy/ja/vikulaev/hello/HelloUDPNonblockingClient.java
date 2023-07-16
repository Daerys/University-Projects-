package info.kgeorgiy.ja.vikulaev.hello;


import java.io.IOException;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

import static info.kgeorgiy.ja.vikulaev.hello.HelloUtils.*;

public class HelloUDPNonblockingClient extends AbstractClient {
    private ByteBuffer[] buffers;
    private AtomicInteger[] progress;
    public static final int TIME_OUT = 100;

    public static void main(String[] args) {
        AbstractClient.main(new HelloUDPNonblockingClient(), args);
    }

    @Override
    protected void runClient(SocketAddress address, ExecutorService executorService) throws IOException {
        Selector selector = Selector.open();
        buffers = new ByteBuffer[THREADS];
        progress = new AtomicInteger[THREADS];
        for (int i = 0; i < THREADS; ++i) {
            DatagramChannel datagramChannel = DatagramChannel.open();
            datagramChannel.configureBlocking(false);
            datagramChannel.connect(address);
            final int buffSize = datagramChannel.getOption(StandardSocketOptions.SO_RCVBUF);
            buffers[i] = ByteBuffer.allocate(buffSize);
            progress[i] = new AtomicInteger(0);
            datagramChannel.register(selector, SelectionKey.OP_WRITE, i);
        }

        while (!Thread.currentThread().isInterrupted() && hasKeys(selector)) {
            int readyChannels = selector.select(TIME_OUT);
            if (readyChannels == 0) {
                selector.keys().forEach(key -> key.interestOpsOr(SelectionKey.OP_WRITE));
            }
            Set<SelectionKey> selectedKeys = selector.selectedKeys();
            Iterator<SelectionKey> keyIterator = selectedKeys.iterator();
            while (keyIterator.hasNext()) {
                SelectionKey key = keyIterator.next();
                keyIterator.remove();

                if (key.isValid()) {
                    selectKey(key);
                }
            }
        }
    }

    private void selectKey(SelectionKey key) {
        if (key.isWritable()) {
            write(key);
        } else if (key.isReadable()) {
            read(key);
        }
    }

    private void write(SelectionKey key) {
        int threadId = (int) key.attachment();
        DatagramChannel channel = (DatagramChannel) key.channel();
        AtomicInteger progressCounter = progress[threadId];
        ByteBuffer buffer = getBuffer(buffers[threadId],
                requestName(threadId + 1, progressCounter.get() + 1));
        try {
            channel.write(buffer);
            key.interestOps(SelectionKey.OP_READ);
        } catch (IOException e) {
            printError(WRITE_ERROR, e.getMessage());
        }
    }

    private ByteBuffer getBuffer(ByteBuffer buffer, String request) {
        buffer.clear();
        buffer.put(request.getBytes(CHARSET));
        buffer.flip();
        return buffer;
    }

    private void read(SelectionKey key) {
        int threadId = (int) key.attachment();
        DatagramChannel channel = (DatagramChannel) key.channel();
        ByteBuffer buffer = buffers[threadId];
        AtomicInteger progressCounter = progress[threadId];

        buffer.clear();

        try {
            channel.receive(buffer);
        } catch (IOException e) {
            printError(READ_ERROR, e.getMessage());
        }
        String response = new String(buffer.array(), buffer.arrayOffset(), buffer.position(), CHARSET);

        if (!responseReceived(response, requestName(threadId + 1, progressCounter.get() + 1))) {
            return;
        }
        System.out.println(response);
        int progress = progressCounter.incrementAndGet();
        if (progress == REQUESTS) {
            try {
                channel.close();
            } catch (IOException e) {
                printError(READ_ERROR, e.getMessage());
            }
            key.cancel();
        }
    }
}


