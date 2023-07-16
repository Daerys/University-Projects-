package info.kgeorgiy.ja.vikulaev.walk;

import java.io.*;
import java.math.BigInteger;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.attribute.BasicFileAttributes;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class FileHashing extends SimpleFileVisitor<Path> {

    private final BufferedWriter output;
    private final int BUFFER_LENGTH = 1 << 10;
    private int HASH_LENGTH = 1 << 6;
    private String INVALID_FILE_HASH = "0".repeat(HASH_LENGTH);
    private static class FileHashingException extends Exception {
        public FileHashingException(String message) {
            super(message);
        }
    }


    public FileHashing(BufferedWriter output) {
        this.output = output;
    }

    private void writeHash(String hash, Path file) throws IOException {
        output.write(hash + " " + file);
        output.newLine();
    }

    @Override
    public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs) throws IOException {
        try {
            if (Files.notExists(dir)) {
                throw new FileHashingException("This directory does not exist: " + dir);
            }
            if (!Files.isReadable(dir)) {
                throw new FileHashingException("Program has no permission to access to this directory: " + dir);
            }
        } catch (FileHashingException e) {
            System.out.println(e.getMessage());
            return FileVisitResult.TERMINATE;
        }
        if (Files.isHidden(dir)) {
            return FileVisitResult.SKIP_SUBTREE;
        }
        File file = new File(dir.toString());
        if (file.length() == 0) {
            writeHash(INVALID_FILE_HASH, dir);
        }
        return FileVisitResult.CONTINUE;
    }

    @Override
    public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
        try (InputStream inputStream = Files.newInputStream(file)) {
            MessageDigest hash = MessageDigest.getInstance("SHA-256");
            byte[] buffer = new byte[BUFFER_LENGTH];
            int c;
            while ((c = inputStream.read(buffer)) != -1) {
                hash.update(buffer, 0, c);
            }
            HASH_LENGTH = hash.getDigestLength() << 1;
            INVALID_FILE_HASH = "0".repeat(HASH_LENGTH);
            writeHash(String.format("%0" + HASH_LENGTH + "x", new BigInteger(1, hash.digest())), file);

        } catch (NoSuchAlgorithmException | IOException ignore) {
            writeHash(INVALID_FILE_HASH, file);
        }
        return FileVisitResult.CONTINUE;
    }

    @Override
    public FileVisitResult visitFileFailed(Path file, IOException exc) throws IOException {
        writeHash(INVALID_FILE_HASH, file);
        return FileVisitResult.CONTINUE;
    }
}
