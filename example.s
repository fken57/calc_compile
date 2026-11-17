.data
L_fmt:
	.ascii "%lld\n\0"
.text
.section .rodata
.Linput_fmt:
	.asciz "%lld"
.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq $0, %rax
	subq $16, %rsp
	leaq .Linput_fmt(%rip), %rdi
	movq %rsp,%rsi
	xorl %eax,%eax
	call scanf
	cmpl $1 , %eax
	je 1f
	 movl $1,%edi
	call exit
1:
	movq (%rsp), %rax
	addq $16, %rsp
	movq %rax, -8(%rbp)
	subq $16, %rsp
	leaq .Linput_fmt(%rip), %rdi
	movq %rsp,%rsi
	xorl %eax,%eax
	call scanf
	cmpl $1 , %eax
	je 1f
	 movl $1,%edi
	call exit
1:
	movq (%rsp), %rax
	addq $16, %rsp
	movq %rax, -16(%rbp)
	movq -8(%rbp), %rax
pushq %rax
	movq $5, %rax
movq %rax, %rdi
popq %rax
	cmpq %rdi, %rax
	setg %al
	movzbq %al, %rax
	cmpq $0, %rax
	je else_0
	movq -8(%rbp), %rax
pushq %rax
	movq -16(%rbp), %rax
movq %rax, %rdi
popq %rax
	imulq %rdi, %rax
	leaq L_fmt(%rip), %rdi
	movq %rax, %rsi
	movq $0 , %rax
	call printf
jmp end_0
else_0:
	movq -16(%rbp), %rax
	leaq L_fmt(%rip), %rdi
	movq %rax, %rsi
	movq $0 , %rax
	call printf
end_0:
	movl $0, %eax
	leave
	ret
