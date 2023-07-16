package info.kgeorgiy.ja.vikulaev.arrayset;

import java.util.*;


public class ArraySet<T> extends AbstractSet<T> implements SortedSet<T> {
    private final ArrayList<T> data;
    private final Comparator<? super T> comparator;
    private final String BORDER_MESSAGE_EXCEPTION = "Subset can't be extracted. " +
            "From element has to be equal of bigger than to element by theirs comparator.";

    public ArraySet() {
        this(new ArrayList<>(), null);
    }

    public ArraySet(Collection<? extends T> collection) {
        this(new ArrayList<>(new TreeSet<>(collection)), null);
    }

    public ArraySet(Comparator<? super T> comparator) {
        this(new ArrayList<>(), comparator);
    }

    public ArraySet(Collection<? extends T> collection, Comparator<? super T> comparator) {
        SortedSet<T> tmpSet = new TreeSet<>(comparator);
        tmpSet.addAll(collection);
        this.data = new ArrayList<>(tmpSet);
        this.comparator = comparator;
    }

    public ArraySet(ArrayList<T> data, Comparator<? super T> comparator) {
        this.data = data;
        this.comparator = comparator;
    }

    public ArraySet(ArraySet<T> set) {
        this(set.data, set.comparator);
    }

    @Override
    public Comparator<? super T> comparator() {
        return comparator;
    }

    private SortedSet<T> subSetById(int fromElementId, int toElementId) {
        if (isEmpty()) {
            return this;
        } else if (fromElementId == toElementId) {
            // In subset "If fromElement and toElement are equal, the returned set is empty".
            return new ArraySet<>(comparator);
        }

        return new ArraySet<>(data.subList(fromElementId, toElementId), comparator);
    }

    @SuppressWarnings("unchecked")
    private void borderValidation(T fromElement, T toElement) {
        if (comparator == null && ((Comparable) fromElement).compareTo(toElement) > 0) {
            throw new IllegalArgumentException(BORDER_MESSAGE_EXCEPTION);
        }
        if (comparator != null && comparator.compare(fromElement, toElement) > 0) {
            throw new IllegalArgumentException(BORDER_MESSAGE_EXCEPTION);
        }
    }

    @Override
    public SortedSet<T> subSet(T fromElement, T toElement) {
        borderValidation(fromElement, toElement);
        return subSetById(getIndex(fromElement), getIndex(toElement));
    }

    @Override
    public SortedSet<T> headSet(T toElement) {
        return subSetById(0, getIndex(toElement));
    }

    @Override
    public SortedSet<T> tailSet(T fromElement) {
        return subSetById(getIndex(fromElement), size());
    }

    @Override
    public T first() {
        assertNotEmpty();
        return data.get(0);
    }

    @Override
    public T last() {
        assertNotEmpty();
        return data.get(size() - 1);
    }

    @Override
    public int size() {
        return data.size();
    }

    @SuppressWarnings("unchecked")
    @Override
    public boolean contains(Object o) {
        return Collections.binarySearch(data, (T) Objects.requireNonNull(o), comparator) >= 0;
    }

    @Override
    public Iterator<T> iterator() {
        return Collections.unmodifiableList(data).iterator();
    }

    private void assertNotEmpty() {
        if (isEmpty()) {
            throw new NoSuchElementException("This set is empty. This function cannot be executed.");
        }
    }

    private int getIndex(final T elem) {
        int searchRes = Collections.binarySearch(data, elem, comparator);
        return (searchRes >= 0) ? searchRes : -(searchRes + 1);
    }
}
