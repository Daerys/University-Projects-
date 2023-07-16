package info.kgeorgiy.ja.vikulaev.bank.Bank;

import info.kgeorgiy.ja.vikulaev.bank.Account.Account;
import info.kgeorgiy.ja.vikulaev.bank.Account.LocalAccount;
import info.kgeorgiy.ja.vikulaev.bank.Account.RemoteAccount;
import info.kgeorgiy.ja.vikulaev.bank.Person.LocalPerson;
import info.kgeorgiy.ja.vikulaev.bank.Person.Person;
import info.kgeorgiy.ja.vikulaev.bank.Person.RemotePerson;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class RemoteBank implements Bank {
    private final int port;
    private final ConcurrentMap<String, RemotePerson> persons = new ConcurrentHashMap<>();
    private final ConcurrentMap<String, Account> accounts = new ConcurrentHashMap<>();

    public RemoteBank(final int port) {
        this.port = port;
    }

    @Override
    public Account createAccount(final String id, final String passportID) throws RemoteException {
        String fullName = BankUtils.fullName(id, passportID);
        System.out.println("Creating account " + id);
        Account account = accounts.get(fullName);
        if (account != null) {
            return account;
        }
        account = new RemoteAccount(id, passportID);
        accounts.put(fullName, account);
        UnicastRemoteObject.exportObject(account, port);
        return account;
    }

    @Override
    public Account getAccount(final String id) throws RemoteException {
        System.out.println("Retrieving account " + id);
        Account account = accounts.get(id);
        if (account == null) {
            String[] creds = id.split(":");
            if (creds.length != 2) {
                return null;
            }
            account = createAccount(creds[1], creds[0]);
        }
        return account;
    }

    @Override
    public Person createPerson(final String firstname, final String lastname, final String passport) {
        System.out.printf("Creating person for %s %s%n", firstname, lastname);
        if (!BankUtils.checkPersonCredentials(firstname, lastname, passport)) {
            System.err.println(BankUtils.CREDENTIALS_ERROR);
            return null;
        }
        try {
            return persons.computeIfAbsent(passport, k -> {
                try {
                    return RemotePerson.newRemotePerson(firstname, lastname, passport, port, this);
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
            });
        } catch (RuntimeException e) {
            System.err.println("Unable to make a person with this input. " + e.getMessage());
            return null;
        }
    }

    @Override
    public RemotePerson getRemotePerson(final String passport) {
        return persons.get(passport);
    }

    @Override
    public LocalPerson getLocalPerson(final String passport) {
        if (!personExists(passport)) {
            return null;
        }
        Person person = persons.get(passport);
        try {

            Map<String, LocalAccount> localAccounts = new HashMap<>();

            accounts.forEach((str, acc) -> {
                try {
                    if (Objects.equals(acc.getPassportID(), passport)) {
                        localAccounts.put(str, new LocalAccount(acc));
                    }
                } catch (RemoteException ignored) {
                }
            });

            return LocalPerson.newLocalPerson(person.getFirstname(), person.getLastname(), person.getPassportID(),
                    localAccounts);
        } catch (RemoteException e) {
            System.err.println("Unable to get local person for that passport. " + e.getMessage());
            return null;
        }
    }

    @Override
    public boolean accountExists(final String id) {
        return accounts.containsKey(id);
    }

    private boolean personExists(final String passport) {
        return persons.containsKey(passport);
    }
}
