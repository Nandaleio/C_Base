<script setup lang="ts">
import {
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogTitle,
} from '@/components/ui/alert-dialog'


const props = defineProps<{
        description: string
        action: () => any;
    }>()

async function confirmFn() {
  await props.action();
  emit('confirm');
}

const emit = defineEmits<{
  cancel: []
  confirm: []
}>()
</script>

<template>
    <AlertDialogContent>
      <AlertDialogHeader>
        <AlertDialogTitle>Are you absolutely sure?</AlertDialogTitle>
        <AlertDialogDescription>
          {{description}}.
        </AlertDialogDescription>
      </AlertDialogHeader>
      <AlertDialogFooter>
        <AlertDialogCancel @click="emit('cancel')">Cancel</AlertDialogCancel>
        <AlertDialogAction @click="confirmFn">Continue</AlertDialogAction>
      </AlertDialogFooter>
    </AlertDialogContent>
</template>