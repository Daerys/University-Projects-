package info.kgeorgiy.ja.vikulaev.walk;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;

public class Walk {

    private static boolean correctParameters(String[] args) {
        return args != null && args.length == 2 && args[0] != null && args[1] != null;
    }

    public static void main(String[] args) {

        if (!correctParameters(args)) {
            System.err.println("Incorrect input format. Example of the correct format: <input file> <output file>");
            return;
        }

        final Path outputFilePath;
        try {
            outputFilePath = Path.of(args[1]);

            if (outputFilePath.getParent() != null) {
                try {
                    Files.createDirectories(outputFilePath.getParent());
                } catch (IOException e) {
                    System.err.printf("Unable to create an output file. %s%n%s%n", args[1], e.getMessage());
                    return;
                }
            }
        } catch (InvalidPathException e) {
            System.err.printf("Wrong path of output file. %s%n%s%n", args[1], e.getMessage());
            return;
        }

        final Path inputFilePath;

        try {
            inputFilePath = Path.of(args[0]);
        } catch (InvalidPathException e) {
            System.err.printf("Path to input file is incorrect. %s%n%s%n", args[0], e.getMessage());
            return;
        }

        try (BufferedReader reader = Files.newBufferedReader(inputFilePath)) {
            try (BufferedWriter writer = Files.newBufferedWriter(outputFilePath)) {
                String string;
                FileHashing fileWalker = new FileHashing(writer);
                while ((string = reader.readLine()) != null) {
                    try {
                        Path path = Path.of(string);
                        Files.walkFileTree(path, fileWalker);
                    } catch (InvalidPathException e) {
                        System.err.printf("Given path %n%s%n contains invalid characters or path string is invalid by system reasons. %n%s", string, e.getMessage());
                        writer.write("0".repeat(64) + " " + string);
                        writer.newLine();
                    }
                }
            } catch (IOException e) {
                System.err.printf("Cannot write into file. %s%n%s%n", args[1], e.getMessage());
            } catch (SecurityException e) {
                System.err.printf("Program has no access to output file. %s%n%s%n", args[1], e.getMessage());
            }
        } catch (IOException e) {
            System.err.printf("Cannot read from file. %s%n%s%n", args[0], e.getMessage());
        } catch (SecurityException e) {
            System.err.printf("Program has no access to input file. %s%n%s%n", args[0], e.getMessage());
        } catch (InvalidPathException e) {
            System.err.printf("Path to input file is incorrect. %s%n%s%n", args[0], e.getMessage());
        }
    }
}

