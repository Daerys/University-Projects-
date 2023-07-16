package info.kgeorgiy.ja.vikulaev.hello;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import static info.kgeorgiy.ja.vikulaev.hello.HelloUtils.printError;

public class HelloUDPServer extends AbstractServer {
    private DatagramSocket serverSocket;


    public static void main(String[] args) {
        AbstractServer.main(new HelloUDPServer(), args);
    }

    @Override
    protected void serverStart() {
        try {
            serverSocket = new DatagramSocket(PORT);
        } catch (SocketException e) {
            printError("Unable to run the server with given port!!", e.getMessage());
            return;
        } catch (IllegalArgumentException e) {
            printError("Unable to run the server with given number of threads or ports!", e.getMessage());
            return;
        }

        receiver.submit(() -> {
            final byte[] buffer;
            try {
                buffer = new byte[serverSocket.getSendBufferSize()];
            } catch (SocketException e) {
                printError("Unable to create packet to port № " + PORT, e.getMessage());
                return;
            }
            while (!serverSocket.isClosed() && !Thread.currentThread().isInterrupted()) {
                DatagramPacket receivePacket = receive(PORT, buffer);
                if (receivePacket == null) continue;
                execute(PORT, receivePacket);
            }
        });
    }

    private DatagramPacket receive(int port, byte[] buffer) {
        DatagramPacket receivePacket;
        receivePacket = new DatagramPacket(buffer, buffer.length);
        try {
            serverSocket.receive(receivePacket);
        } catch (IOException e) {
            printError("Unable to receive request from port № " + port, e.getMessage());
            return null;
        }
        return receivePacket;
    }

    private void execute(int port, DatagramPacket receivePacket) {
        String request;
        request = getRequest(receivePacket);
        workers.submit(() -> {
            String prefix = "Hello, ";
            byte[] message = (prefix + request).getBytes();
            DatagramPacket sendPacket = new DatagramPacket(
                    message, message.length,
                    receivePacket.getAddress(), receivePacket.getPort());
            try {
                serverSocket.send(sendPacket);
            } catch (IOException e) {
                printError("Unable to receive request from port № " + port, e.getMessage());
            }
        });
    }


    private String getRequest(DatagramPacket receivePacket) {
        return new String(receivePacket.getData(), receivePacket.getOffset(),
                receivePacket.getLength(), CHARSET);
    }

    @Override
    public void close() {
        super.close();
        serverSocket.close();
    }
}
