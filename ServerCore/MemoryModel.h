#pragma once
/*
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

atomic<bool> ready;
int32 value;

void Producer()
{
	value = 10;

	ready.store(true, memory_order::memory_order_seq_cst); // = ready.store(true);
	//ready.store(true, memory_order::memory_order_release);
	//std::atomic_thread_fence(memory_order::memory_order_release);
	// ---------------- ���뼱 -----------------
}

void Consumer()
{
	// ---------------- ���뼱 -----------------
	//std::atomic_thread_fence(memory_order::memory_order_acquire);
	//while (ready.load(memory_order::memory_order_acquire) == false);
	while (ready.load(memory_order::memory_order_seq_cst) == false); // = ready.load();

	cout << value << endl;
}

void main()
{
	ready = false;
	value = 0;
	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	// Memory Model (��å)
	// 1) Sequentially Consistent (seq_cst)
	// 2) Acquire-Release (consume, acquire, release, acq_rel[acquire + release]) *consume�� �̽��� ����
	// 3) Relaxed (relaxed)

	// 1) seq_cst (���� ���� = �����Ϸ� ����ȭ ���� ���� = ������)
	// ���ü� ���� �ٷ� �ذ�, �ڵ� ���ġ �ٷ� �ذ�

	// 2) acquire-Release
	// �� �߰�
	// release ��� ������ �޸� ��ɵ��� �ش� ��� ���ķ� ���ġ �Ǵ� ���� ����
	// �׸��� acquire�� ���� ������ �д� �����尡 �ִٸ�
	// release ������ ��ɵ��� -> acquire �ϴ� ������ ���� �����ϴ�. (���ü� ����)

	// 3) relaxed (�����Ӵ� = �����Ϸ� ����ȭ ���� ���� = ���������� ����)
	// �ʹ����� ����ϰ� �����Ӵ�.
	// �ڵ� ���ġ�� �ڴ�� ����, ���ü� �ذ� �Ұ���
	// ���� �⺻ ���� (���� ��ü�� ���� ���� ���� ����)�� ����

	// ����, AMD�� ��� �ִ��� ������ �ϰ����� ������ �ؼ�,
	// seq_cst�� �ᵵ ���ٸ� ���ϰ� ����.
	// ARM�� ��� �� ���̰� �ִٰ� �Ѵ�.
	// NC���� �������� ���� ��찡 �ִ�.
}
*/