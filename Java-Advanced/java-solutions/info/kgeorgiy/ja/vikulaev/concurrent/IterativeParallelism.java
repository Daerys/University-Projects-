package info.kgeorgiy.ja.vikulaev.concurrent;

import info.kgeorgiy.java.advanced.concurrent.ScalarIP;
import info.kgeorgiy.java.advanced.mapper.ParallelMapper;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.IntStream;
import java.util.stream.Stream;

public class IterativeParallelism implements ScalarIP {

    /**
     * inner Runnable class which is holding current thread, result and thread's task.
     *
     * @param <T>
     * @param <E>
     */
    private static class MyRunnable<T, E> implements Runnable {

        /**
         * List of values on which function applies
         */
        private final List<? extends T> values;
        /**
         * Current thread task
         */
        private final Function<Stream<? extends T>, ? extends E> function;
        /**
         * Current thread
         */
        private final Thread thread;
        /**
         * Result of task
         */
        private E result;

        MyRunnable(final List<? extends T> values, final Function<Stream<? extends T>, ? extends E> function) {
            this.values = values;
            this.function = function;
            this.thread = new Thread(this);
        }

        @Override
        public void run() {
            result = function.apply(values.stream());
        }

        /**
         * Returns result of current thread task
         *
         * @return result of current thread task
         */

        public E getResult() {
            return result;
        }

        /**
         * Starting current thread
         */

        public void start() {
            thread.start();
        }

        /**
         * Joins current thread
         *
         * @throws InterruptedException if it's unable to join this thread
         */
        public void join() throws InterruptedException {
            thread.join();
        }

        /**
         * interrupts current thread
         */
        public void interrupt() {
            thread.interrupt();
        }
    }

    /**
     * Inner ParallelMapper
     */
    private final ParallelMapper parallelMapper;

    /**
     * Constructor from ParallelMapper
     *
     * @param parallelMapper ParallelMapper
     */

    public IterativeParallelism(ParallelMapper parallelMapper) {
        this.parallelMapper = parallelMapper;
    }

    /**
     * Empty constructor
     */
    public IterativeParallelism() {
        parallelMapper = null;
    }

    /**
     * Main body of class. It's generates threads itself or runs by ParallelMapper
     *
     * @param threads number of threads
     * @param values  list of values function applies to
     * @param applyer function to apply on list
     * @param <T>     parameter of list values
     * @param <E>     parameter of return values
     * @return stream of results of each thread
     * @throws InterruptedException if values is null or treads < 1 or it's unable to join all threads.
     */
    private <T, E> Stream<? extends E> threading(int threads,
                                                 List<? extends T> values,
                                                 Function<Stream<? extends T>, ? extends E> applyer) throws InterruptedException {
        if (threads < 1) {
            throw new IllegalArgumentException(String.format("Number of threads must be at least one. %d were given", threads));
        }
        if (values == null) {
            throw new IllegalArgumentException("Can't process a list. List must be not null.");
        }

        int chunkSize = Math.max(1, (int) Math.ceil((double) values.size() / threads));
        Stream<List<? extends T>> chunks = IntStream.range(0, (values.size() + chunkSize - 1) / chunkSize)
                .mapToObj(i -> values.subList(i * chunkSize, Math.min((i + 1) * chunkSize, values.size())));

        Stream<? extends E> res;

        if (parallelMapper != null) {
            List<Stream<? extends T>> streams = new ArrayList<>();
            chunks.forEach(chunk -> streams.add(chunk.stream()));
            res = new ArrayList<>(parallelMapper.map(applyer, streams)).stream();
        } else {
            List<MyRunnable<T, E>> tasks = new ArrayList<>();

            chunks.forEach(chunk -> {
                MyRunnable<T, E> runnable = new MyRunnable<>(chunk, applyer);
                tasks.add(runnable);
                runnable.start();
            });

            for (MyRunnable<T, E> task : tasks) {
                try {
                    task.join();
                } catch (InterruptedException e) {
                    tasks.subList(tasks.indexOf(task), threads).forEach(MyRunnable::interrupt);
                    throw e;
                }
            }

            res = tasks.stream().map(MyRunnable::getResult);
        }

        return res;
    }

    @Override
    @SuppressWarnings("OptionalGetWithoutIsPresent")
    public <T> T maximum(int threads, List<? extends T> values, Comparator<? super T> comparator) throws InterruptedException {
        return threading(threads, values, stream -> stream.max(comparator).get()).max(comparator).get();
    }

    @Override
    public <T> T minimum(int threads, List<? extends T> values, Comparator<? super T> comparator) throws InterruptedException {
        return maximum(threads, values, comparator.reversed());
    }

    @Override
    public <T> boolean all(int threads, List<? extends T> values, Predicate<? super T> predicate) throws InterruptedException {
        return threading(threads, values, stream -> stream.allMatch(predicate)).allMatch(Boolean::booleanValue);
    }

    @Override
    public <T> boolean any(int threads, List<? extends T> values, Predicate<? super T> predicate) throws InterruptedException {
        return !all(threads, values, predicate.negate());
    }

    @Override
    public <T> int count(int threads, List<? extends T> values, Predicate<? super T> predicate) throws InterruptedException {
        return threading(threads, values, stream -> stream.filter(predicate).count())
                .mapToInt(Long::intValue).sum();
    }
}
