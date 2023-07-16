package info.kgeorgiy.ja.vikulaev.hello;

import java.io.IOException;
import java.net.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.stream.IntStream;

public class HelloUDPClient extends AbstractClient {

    public static void main(String[] args) {
        AbstractClient.main(new HelloUDPClient(), args);
    }

    @Override
    protected void runClient(SocketAddress address, ExecutorService executorService) {

        IntStream.range(0, THREADS).forEach(thread ->
                executorService.submit(() -> execute(thread, REQUESTS, address)));

        executorService.shutdownNow();
        try (executorService) {
            if (!executorService.awaitTermination((long) THREADS * REQUESTS, TimeUnit.MINUTES)) {
                HelloUtils.printError("Some tasks were not terminated.", "");
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            HelloUtils.printError("Interrupted while waiting for task termination.", "");
        }

    }

    private void execute(int thread, int requests, SocketAddress socketAddress) {
        try (DatagramSocket socket = new DatagramSocket()) {
            socket.setSoTimeout(SOCKET_TIMEOUT);
            byte[] buffer = new byte[socket.getReceiveBufferSize()];
            IntStream.range(0, requests).forEach(request ->
                    sendAndFetch(socket, new DatagramPacket(buffer, buffer.length), socketAddress,
                            thread, request));
        } catch (SocketException e) {
            HelloUtils.printError("The socket couldn't be opened.", e.getMessage());
        }
    }

    private void sendAndFetch(DatagramSocket socket, DatagramPacket response, SocketAddress socketAddress,
                              int thread, int request) {
        String name = requestName(thread + 1, request + 1);
        DatagramPacket packet = new DatagramPacket(name.getBytes(CHARSET), name.length(), socketAddress);
        String result = "";
        do {
            try {
                socket.send(packet);
            } catch (IOException e) {
                HelloUtils.printError(READ_ERROR, e.getMessage());
                continue;
            }
            try {
                socket.receive(response);
            } catch (SocketTimeoutException e) {
                HelloUtils.printError("Didn't received the response.", e.getMessage());
                continue;
            } catch (IOException e) {
                HelloUtils.printError(WRITE_ERROR, e.getMessage());
                continue;
            }
            result = getResult(response);
        } while (!HelloUtils.responseReceived(result, name));
        System.out.println(result);
    }

    private String getResult(DatagramPacket response) {
        return new String(response.getData(), response.getOffset(), response.getLength(), CHARSET);
    }

}

