package info.kgeorgiy.ja.vikulaev.concurrent;


import info.kgeorgiy.java.advanced.mapper.ParallelMapper;

import java.util.*;
import java.util.function.Function;
import java.util.function.Supplier;

public class ParallelMapperImpl implements ParallelMapper {

    /**
     * Variable which represents is ParallelMapperImpl closed or not
     */
    private volatile boolean isClosed = false;
    /**
     * List of threads
     */
    private final List<Thread> threads;
    /**
     * Queue of tasks
     */
    private final Queue<Runnable> tasks;

    /**
     * Class constructor which generates threadCount threads and makes an empty queue.
     *
     * @param threadCount is a number of threads
     */

    public ParallelMapperImpl(final int threadCount) {
        this.threads = new ArrayList<>(threadCount);
        this.tasks = new ArrayDeque<>();

        for (int i = 0; i < threadCount; i++) {
            Thread thread = new Thread(() -> {
                try {
                    while (!Thread.currentThread().isInterrupted()) {
                        Runnable task;
                        synchronized (tasks) {
                            while (tasks.isEmpty() && !Thread.currentThread().isInterrupted()) {
                                tasks.wait();
                            }
                            if (tasks.isEmpty()) {
                                continue;
                            }
                            task = tasks.remove();
                        }
                        task.run();
                    }
                } catch (InterruptedException ignored) {
                } finally {
                    Thread.currentThread().interrupt();
                }
            });
            thread.start();
            threads.add(thread);
        }
    }

    @Override
    public <T, R> List<R> map(Function<? super T, ? extends R> f, List<? extends T> args) throws InterruptedException {
        if (isClosed) {
            throw new IllegalStateException("ParallelMapper has already been closed.");
        }
        List<R> results = new ArrayList<>(Collections.nCopies(args.size(), null));
        int remainingTasks = args.size();
        final int[] completedTasks = {0};

        final RuntimeException[] exception = new RuntimeException[1];

        List<Supplier<R>> tasksList = args.stream().map(arg -> (Supplier<R>) () -> {
                    R result = null;
                    try {
                        result = f.apply(arg);
                    } catch (RuntimeException e) {
                        exception[0] = e;
                    }
                    return result;
                }
        ).toList();

        if (exception[0] != null) {
            throw exception[0];
        }

        for (int i = 0; i < tasksList.size(); ++i) {
            var task = tasksList.get(i);
            synchronized (tasks) {
                int finalI = i;
                tasks.add(() -> {
                    R result = task.get();
                    synchronized (completedTasks) {
                        results.set(finalI, result);
                        completedTasks[0]++;
                        completedTasks.notifyAll();
                    }
                });
                tasks.notify();
            }
        }

        synchronized (completedTasks) {
            while (completedTasks[0] < remainingTasks) {
                completedTasks.wait();
            }
        }

        return results;
    }

    @Override
    public void close() {
        isClosed = true;
        threads.forEach(Thread::interrupt);
        threads.forEach(thread -> {
                    try {
                        thread.join();
                    } catch (InterruptedException ignored) {
                    }
                }
        );
    }
}
