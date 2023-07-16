package info.kgeorgiy.ja.vikulaev.bank.Account;

import java.rmi.*;

public interface Account extends Remote {

    String getPassportID() throws RemoteException;

    /** Returns account identifier. */
    String getId() throws RemoteException;

    /** Returns amount of money in the account. */
    int getAmount() throws RemoteException;

    /** Sets amount of money in the account. */
    void setAmount(int amount) throws RemoteException;
}
