package info.kgeorgiy.ja.vikulaev.hello;

import java.nio.channels.Selector;

public class HelloUtils {
    public static void printError(String message, String error) {
        System.err.printf("%s %s%n", message, error);
    }

    public static String requestName(String prefix, int thread, int request) {
        return String.format("%s%d_%d", prefix, thread, request);
    }

    public static boolean responseReceived(String result, String name) {
        return result.contains(name);
    }


    public static boolean hasKeys(Selector selector) {
        return !selector.keys().isEmpty();
    }
}
