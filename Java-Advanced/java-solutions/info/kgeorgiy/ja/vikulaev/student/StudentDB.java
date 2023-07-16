package info.kgeorgiy.ja.vikulaev.student;

import info.kgeorgiy.java.advanced.student.GroupName;
import info.kgeorgiy.java.advanced.student.Student;
import info.kgeorgiy.java.advanced.student.StudentQuery;

import java.util.*;
import java.util.function.BinaryOperator;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class StudentDB implements StudentQuery {

    @Override
    public List<String> getFirstNames(List<Student> students) {
        return getListOfStudentsDetails(students, Student::getFirstName);
    }

    @Override
    public List<String> getLastNames(List<Student> students) {
        return getListOfStudentsDetails(students, Student::getLastName);
    }

    @Override
    public List<GroupName> getGroups(List<Student> students) {
        return getListOfStudentsDetails(students, Student::getGroup);
    }

    @Override
    public List<String> getFullNames(List<Student> students) {
        return getListOfStudentsDetails(students, GET_FULL_NAME);
    }

    @Override
    public Set<String> getDistinctFirstNames(List<Student> students) {
        return students.stream().map(Student::getFirstName)
                .collect(Collectors.toCollection(TreeSet::new));
    }

    @Override
    public String getMaxStudentFirstName(List<Student> students) {
        return students.stream().max(Student::compareTo).map(Student::getFirstName).orElse("");
    }

    @Override
    public List<Student> sortStudentsById(Collection<Student> students) {
        return sortStudentsByAbstractValue(students, Student::compareTo);
    }

    @Override
    public List<Student> sortStudentsByName(Collection<Student> students) {
        return sortStudentsByAbstractValue(students, STUDENT_COMPARATOR);
    }

    @Override
    public List<Student> findStudentsByFirstName(Collection<Student> students, String name) {
        return getStream(students, name, Student::getFirstName).collect(Collectors.toList());
    }

    @Override
    public List<Student> findStudentsByLastName(Collection<Student> students, String name) {
        return getStream(students, name, Student::getLastName).collect(Collectors.toList());
    }

    @Override
    public List<Student> findStudentsByGroup(Collection<Student> students, GroupName group) {
        return getStream(students, group, Student::getGroup).collect(Collectors.toList());
    }

    @Override
    public Map<String, String> findStudentNamesByGroup(Collection<Student> students, GroupName group) {
        return getStream(students, group, Student::getGroup)
                .collect(Collectors.toMap(
                        Student::getLastName,
                        Student::getFirstName,
                        BinaryOperator.minBy(String::compareTo)
                ));
    }

    private List<Student> sortStudentsByAbstractValue(Collection<Student> students, Comparator<Student> comparator) {
        return students.stream().sorted(comparator).collect(Collectors.toList());
    }


    private <T> List<T> getListOfStudentsDetails(final List<Student> students, final Function<? super Student, T> function) {
        return students.stream().map(function).collect(Collectors.toList());
    }

    private Stream<Student> getStreamByAbstractPredicate(final Collection<Student> students, final Predicate<Student> filter) {
        return students.stream().filter(filter).sorted(STUDENT_COMPARATOR);
    }

    private <T> Stream<Student> getStream(Collection<Student> students, T compareTo, Function<Student, T> function) {
        return getStreamByAbstractPredicate(students, generatePredicate(compareTo, function));
    }

    private <T> Predicate<Student> generatePredicate(T equalsTo, final Function<Student, T> filterFunction) {
        return student -> equalsTo.equals(filterFunction.apply(student));
    }

    private static final Function<Student, String> GET_FULL_NAME =
            (final Student student) -> String.format("%s %s", student.getFirstName(), student.getLastName());
    private static final Comparator<Student> STUDENT_COMPARATOR =
            Comparator.comparing(Student::getLastName, Comparator.reverseOrder())
                    .thenComparing(Student::getFirstName, Comparator.reverseOrder())
                    .thenComparing(Student::getId);
}
