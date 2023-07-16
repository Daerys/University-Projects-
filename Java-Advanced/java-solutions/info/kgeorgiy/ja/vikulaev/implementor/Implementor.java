package info.kgeorgiy.ja.vikulaev.implementor;


import info.kgeorgiy.java.advanced.implementor.*;

import javax.tools.*;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.lang.reflect.Parameter;
import java.net.URISyntaxException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.util.*;
import java.util.jar.Attributes;
import java.util.jar.JarOutputStream;
import java.util.jar.Manifest;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import java.util.zip.ZipEntry;


/**
 * Implementation class for {@link JarImpler} interface
 */
public class Implementor implements JarImpler {

    /**
     * Creates new instance of {@link Implementor}
     */
    public Implementor() {
    }

    /**
     * Main method is used to choose which way to execute {@link Implementor}
     * <ul>
     *     <li>2 arguments: className rootPath - runs {@link #implement(Class, Path)}</li>
     *     <li>3 arguments: -jar className rootPath - runs {@link #implementJar(Class, Path)}</li>
     * </ul>
     *
     * @param args arguments for running application
     * @throws ImplerException if error occurs during execution such as:
     *
     *                         <ul>
     *                             <li>Incorrect input format</li>
     *                             <li>Path to file is incorrect</li>
     *                             <li>Interface name if invalid</li>
     *                             <li>Other problems in {@link  #implement(Class, Path)} or {@link  #implementJar(Class, Path)} method</li>
     *                         </ul>
     */
    public static void main(String[] args) throws ImplerException {
        checkArgs(args);

        int offset = (args[0].equals("-jar")) ? 1 : 0;

        Class<?> token;
        Path path;

        try {
            path = Path.of(String.valueOf(args[offset + 1]));
        } catch (InvalidPathException e) {
            throw new ImplerException("Invalid path were given.", e);

        }

        try {
            token = Class.forName(args[offset]);
        } catch (ClassNotFoundException e) {
            throw new ImplerException("Invalid interface name " + args[offset], e);
        }

        Implementor implementor = new Implementor();
        if (offset == 0) {
            implementor.implement(token, path);
            return;
        }
        implementor.implementJar(token, path);
    }

    /**
     * Private function. Used to check if given parameters to {@link #main(String[] args)} are correct.
     * <ul>
     *     <li>If {@link Implementor} runs with -jar, then check if args.length == 3 and first arg {@link #equals(Object)} <var>-jar</var> and args[1] != null != args[2]</li>
     *     <li>Else check if args.length == 2 and args[0] != null != args[1]</li>
     * </ul>
     *
     * @param args arguments of {@link #main(String[])}} function
     * @throws ImplerException in case if:
     *                         <ul>
     *                             <li>null array or array.length != 2 and array.length != 3 were given</li>
     *                             <li>One of arguments is null</li>
     *                         </ul>
     * @see java.lang.reflect.Array#getLength(java.lang.Object)
     */
    private static void checkArgs(String[] args) throws ImplerException {
        if (args.length != 2 && args.length != 3) {
            throw new ImplerException("Input has to be ether <Interface Name> <Path to interface> or -jar <Interface Name> <Path to interface> ");
        }
        for (String i : args) {
            if (i == null) {
                throw new ImplerException("Not null arguments are required");
            }
        }
    }

    /**
     * Generates implementation of given interface.
     *
     * @param token interface to create implementation for.
     * @param root  root directory.
     * @throws ImplerException if error occurs during execution such as:
     *                         <ul>
     *                             <li>Some arguments are null</li>
     *                             <li>Given <var>class</var> is not a public interface</li>
     *                             <li>IOException during implementation</li>
     *                         </ul>
     */
    @Override
    public void implement(Class<?> token, Path root) throws ImplerException {
        checkInput(token);
        Path path = generatePath(token, root);
        createDirectory(path);
        generateImplementation(token, path);
    }

    /**
     * Generates <var>.jar</var> of implementation <var>.class</var> files of given interface
     *
     * @param token   type token to create implementation for.
     * @param jarFile target <var>.jar</var> file.
     * @throws ImplerException if error occurs during execution such as:
     *                         <ul>
     *                             <li>Some arguments are null</li>
     *                             <li>Given <var>class</var> is not a public interface</li>
     *                             <li>IOException during implementation</li>
     *                             <li>Couldn't delete temp directories</li>
     *                         </ul>
     */
    @Override
    public void implementJar(Class<?> token, Path jarFile) throws ImplerException {
        Path tmpDir;
        try {
            tmpDir = Files.createTempDirectory(jarFile.getParent(), OUTDIR);
        } catch (IOException e) {
            throw new ImplerException("Unable to make tmp directory.");
        }
        try {
            implement(token, tmpDir);
            compile(token, tmpDir);
            produceJar(token, jarFile, tmpDir);
        } finally {
            try (Stream<Path> stream = Files.walk(tmpDir)) {
                stream.sorted(Comparator.reverseOrder()).map(Path::toFile).forEach(File::delete);
            } catch (IOException e) {
                throw new ImplerException("Exception while deleting temporary dir: " + e.getMessage(), e);
            }
        }
    }

    /**
     * Function which writes ang generates interface implementation
     *
     * @param token is an interface which implementation has to be generated
     * @param path  is a path to <var>.java</var> class where implementation is stored
     */
    private void generateImplementation(Class<?> token, Path path) {
        try (BufferedWriter bufferedWriter = Files.newBufferedWriter(path)) {
            ImplementorWriter writer = new ImplementorWriter(bufferedWriter);
            writer.write(generateHeader(token));

            for (Method method : token.getMethods()) {
                if (method.isDefault()) {
                    continue;
                }
                writer.write(generateMethod(method));
            }
            writer.write("}".concat(NEW_LINE));

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Generates a path to given <var>token</var>
     *
     * @param token is an interface which implementation generates
     * @param root  is a path to <var>.java</var> class where implementation is stored
     * @return path to <var>.java</var> file of given <var>token</var>
     */
    private Path generatePath(Class<?> token, Path root) {
        return root.resolve(token.getPackageName()
                        .replace('.', File.separatorChar))
                .resolve(token.getSimpleName().concat("Impl").concat(".java"));
    }

    /**
     * Generates implementation of given method
     *
     * @param method is a method which implementation generates
     * @return simple implementation of given <var>method</var>
     */
    private String generateMethod(Method method) {

        StringBuilder exceptions = new StringBuilder();

        if (method.getExceptionTypes().length > 0) {
            exceptions.append(" throws ");
            for (var i : method.getExceptionTypes()) {
                exceptions.append(i.getCanonicalName()).append(" ");
            }
        }

        return String.format("%n%s%s %s %s(%s)%s {%s}%n", TABULATION,
                Modifier.toString(method.getModifiers() & ~Modifier.ABSTRACT & ~Modifier.TRANSIENT),
                method.getReturnType().getCanonicalName(),
                method.getName(),
                getMethodParameters(method),
                exceptions,
                getReturnValue(method));
    }

    /**
     * Generates header for <var>.java</var> file of given interface
     *
     * @param token interface which implementation is generates
     * @return header of <var>.java</var> file of given interface
     */
    private String generateHeader(Class<?> token) {
        StringBuilder header = new StringBuilder();

        if (!token.getPackageName().isEmpty()) {
            header.append(String.format("package %s;%n%n", token.getPackage().getName()));
        }

        header.append(String.format("public class %s%s %s %s {%n",
                token.getSimpleName(),
                "Impl",
                "implements",
                getClassName(token)));
        return header.toString();
    }

    /**
     * Generated proper method parameters:
     * <ul>
     *     <li>If variable has not name in interface's declaration it will be named as <var>argi</var></li>
     * </ul>
     *
     * @param method is a method which parameters are generated
     * @return <var>StringJoiner</var> of method parameter's name
     */
    private StringJoiner getMethodParameters(Method method) {
        Parameter[] parameters = method.getParameters();

        StringJoiner joiner = new StringJoiner(", ");
        int index = 0;

        for (Parameter parameter : parameters) {
            String name = parameter.getName();
            if (name == null) {
                name = "arg" + index;
            }
            joiner.add(parameter.getType().getCanonicalName() + " " + name);
            index++;
        }

        return joiner;
    }


    /**
     * Generates a body of simple method implementation
     *
     * @param method ia a method which body is implemented
     * @return method's body implementation
     */
    private String getReturnValue(Method method) {
        return (method.getReturnType() == void.class) ? "" :
                (method.getReturnType().isPrimitive()) ?
                        String.format(" return %s; ", method.getReturnType() == boolean.class ? "false" : "0") :
                        " return null; ";
    }

    /**
     * Extracts full name of given interface
     *
     * @param token is an interface which full name is searched
     * @return full name of given interface
     */
    private String getClassName(Class<?> token) {
        StringBuilder builder = new StringBuilder(token.getSimpleName());
        while ((token = token.getEnclosingClass()) != null) {
            builder.insert(0, token.getSimpleName() + ".");
        }
        return builder.toString();
    }

    /**
     * Creates a directory of the interface implementation.
     *
     * @param path is a parameter where implementation has to be stored
     * @throws ImplerException if unable to create a directory
     */
    private static void createDirectory(Path path) throws ImplerException {
        if (path.getParent() != null) {
            try {
                Files.createDirectories(path.getParent());
            } catch (IOException e) {
                throw new ImplerException("Сan not create directories for output.", e);
            }
        }
    }

    /**
     * Checks if given class is a public interface
     *
     * @param token is the class which were given to implement
     * @throws ImplerException if given class is not public interface
     */
    private void checkInput(Class<?> token) throws ImplerException {
        if (token.isPrimitive() || token.isEnum() || token.isSealed()
                || token == Enum.class || token.isArray()
                || Modifier.isFinal(token.getModifiers()) || token.isAnonymousClass()
                || token.isRecord() || Modifier.isPrivate(token.getModifiers())) {
            throw new ImplerException("Invalid interface.");
        }
    }

    /**
     * Generates <var>.jar</var> file of given interface implementation.
     *
     * @param aClass interface implementation
     * @param path   path where <var>.jar</var> file has to be stored
     * @param tmp    path where interface implementation is stored
     * @throws ImplerException if IOException were caught
     */
    private void produceJar(Class<?> aClass, Path path, Path tmp) throws ImplerException {
        Manifest manifest = new Manifest();
        manifest.getMainAttributes().put(Attributes.Name.MANIFEST_VERSION, "1.0");
        manifest.getMainAttributes().put(Attributes.Name.IMPLEMENTATION_VENDOR, "Ivan Vikulaev");
        String classname = String.format("%s/%s%s", aClass.getPackageName().replace('.', '/'),
                aClass.getSimpleName(), "Impl.class");

        try (JarOutputStream jarOutputStream = new JarOutputStream(Files.newOutputStream(path), manifest)) {
            jarOutputStream.putNextEntry(new ZipEntry(classname));
            Files.copy(tmp.resolve(classname), jarOutputStream);
        } catch (IOException e) {
            throw new ImplerException(String.format("%s has been caught while writhing into jar.", e.getMessage()), e);
        }
    }

    /**
     * Generates <var>.class</var> files of interface implementation.
     *
     * @param token  is the interface implementation.
     * @param tmpDir is a path there {@link Class} is stored
     * @throws ImplerException if:
     *                         <ul>
     *                             <li>System java compiler couldn't be found</li>
     *                             <li>Unable to find interface</li>
     *                             <li>Unable to compile interfaces implementation</li>
     *                         </ul>
     */
    private void compile(Class<?> token, Path tmpDir) throws ImplerException {
        Path filepath = generatePath(token, tmpDir);

        JavaCompiler compiler = ToolProvider.getSystemJavaCompiler();
        if (compiler == null) {
            throw new ImplerException("No Java compiler available");
        }

        Path interfacePath;
        try {
            interfacePath = Path.of(token.getProtectionDomain().getCodeSource().getLocation().toURI());
        } catch (URISyntaxException e) {
            throw new ImplerException("Can not extract path to given interface", e);
        }

        List<String> options = List.of("-cp", tmpDir.getParent().resolve(interfacePath).toString());
        JavaCompiler.CompilationTask task = compiler.getTask(
                null,
                null,
                null,
                options,
                null,
                compiler.getStandardFileManager(
                        null,
                        null,
                        StandardCharsets.UTF_8).getJavaFileObjects(filepath.toFile())
        );
        if (!task.call()) {
            throw new ImplerException("Can not compile: " + filepath);
        }
    }

    /**
     * System line separator
     */
    private static final String NEW_LINE = System.lineSeparator();
    /**
     * System tabulation
     */
    private static final String TABULATION = "\t";

    /**
     * Output temp directory suffix
     */
    public static final String OUTDIR = "tmp";


    /**
     * The ImplementorWriter class is a record that provides a method for writing strings
     * to a BufferedWriter in a format that escapes any non-ASCII characters.
     * This class contains a single method, write, that takes a string as input and writes
     * it to the BufferedWriter provided during instantiation. If the string contains any
     * non-ASCII characters, they are converted to a Unicode escape sequence in the format \\uXXXX.
     *
     * @param writer Inner writer of ImplementorWriter
     */
    private record ImplementorWriter(BufferedWriter writer) {
        /**
         * Constructs an `ImplementorWriter` object with the specified `BufferedWriter`.
         *
         * @param writer the `BufferedWriter` to write to
         */
        private ImplementorWriter {
        }

        /**
         * Writes the specified string to the `BufferedWriter`, escaping any non-ASCII characters
         * as Unicode escape sequences in the format `\\uXXXX`.
         *
         * @param str the string to write
         * @throws IOException if an I/O error occurs
         */
        void write(String str) throws IOException {
            writer.write(str.chars()
                    .mapToObj(c -> c >= 128 ? String.format("\\u%04X", c) : String.valueOf((char) c))
                    .collect(Collectors.joining()));
        }
    }
}
